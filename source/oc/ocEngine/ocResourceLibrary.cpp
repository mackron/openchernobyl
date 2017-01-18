// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocResourceLibraryInit(ocResourceLoader* pLoader, ocGraphicsContext* pGraphics, ocResourceLibrary* pLibrary)
{
    if (pLibrary == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pLibrary);

    if (pLoader == NULL || pGraphics == NULL) return OC_RESULT_INVALID_ARGS;

    pLibrary->pLoader = pLoader;
    pLibrary->pGraphics = pGraphics;

    return OC_RESULT_SUCCESS;
}

void ocResourceLibraryUninit(ocResourceLibrary* pLibrary)
{
    if (pLibrary == NULL) return;
}


OC_PRIVATE ocResource* ocAllocResource(ocResourceType type, size_t payloadSize, const char* absolutePath)
{
    if (absolutePath == NULL) return NULL;

    size_t pathLen = strlen(absolutePath);
    ocResource* pResource = (ocResource*)ocCalloc(1, sizeof(*pResource) + pathLen + payloadSize);
    if (pResource == NULL) {
        return NULL;
    }

    // The first bytes in the payload is always the absolute path of the resource.
    strcpy_s(pResource->_pPayload, pathLen+1, absolutePath);

    pResource->type = type;
    pResource->pAbsolutePath = (const char*)pResource->_pPayload;
    pResource->referenceCount = 1;

    return pResource;
}

OC_PRIVATE ocResult ocResourceLibraryLoad_Image(ocResourceLibrary* pLibrary, const char* absolutePath, ocResource** ppResource)
{
    ocAssert(pLibrary != NULL);
    ocAssert(absolutePath != NULL);
    ocAssert(ppResource != NULL);

    ocImageData data;
    ocResult result = ocResourceLoaderLoadImage(pLibrary->pLoader, absolutePath, &data);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }


    // TODO: This needs a clean up, particularly the mipmap generation stuff.
    dr_bool32 freeImageData = false;
    size_t imageDataSize = data.imageDataSize;
    void* pImageData = data.pImageData;
    uint32_t mipmapCount = data.mipmapCount;
    ocMipmapInfo pMipmaps[32];
    if (mipmapCount == 1) {
        // Generate mipmaps.
        uint32_t baseWidth = data.pMipmaps[0].width;
        uint32_t baseHeight = data.pMipmaps[0].height;
        ocGetMipmapCount(baseWidth, baseHeight, &mipmapCount);
        ocGetTotalMipmapDataSize(baseWidth, baseHeight, 4, sizeof(uintptr_t), &imageDataSize);

        pImageData = malloc(imageDataSize);
        if (pImageData == NULL) {
            ocResourceLoaderUnloadImage(pLibrary->pLoader, &data);
            return OC_RESULT_OUT_OF_MEMORY;
        }

        freeImageData = true;
        ocGenerateMipmaps(baseWidth, baseHeight, 4, sizeof(uintptr_t), data.pImageData, pImageData, pMipmaps);
    } else {
        // Use pre-generated mipmaps.
        memcpy(pMipmaps, data.pMipmaps, mipmapCount * sizeof(ocMipmapInfo));
    }
                

    ocGraphicsImageDesc desc;
    desc.usage = OC_GRAPHICS_IMAGE_USAGE_SHADER_INPUT;
    desc.format = data.format;
    desc.mipLevels = mipmapCount;
    desc.pMipmaps = pMipmaps;
    desc.imageDataSize = imageDataSize;
    desc.pImageData = pImageData;

    ocGraphicsImage* pGraphicsImage;
    result = ocGraphicsCreateImage(pLibrary->pGraphics, &desc, &pGraphicsImage);
    if (result != OC_RESULT_SUCCESS) {
        if (freeImageData) free(pImageData);
        ocResourceLoaderUnloadImage(pLibrary->pLoader, &data);
        return result;
    }

    if (freeImageData) free(pImageData);

    ocResource* pResource = ocAllocResource(ocResourceType_Image, 0, absolutePath);
    if (pResource == NULL) {
        ocResourceLoaderUnloadImage(pLibrary->pLoader, &data);
        return OC_RESULT_OUT_OF_MEMORY;
    }

    pResource->image.pGraphicsImage = pGraphicsImage;

    ocResourceLoaderUnloadImage(pLibrary->pLoader, &data);

    *ppResource = pResource;
    return result;
}

ocResult ocResourceLibraryLoad(ocResourceLibrary* pLibrary, const char* filePath, ocResource** ppResource)
{
    if (ppResource == NULL) return OC_RESULT_INVALID_ARGS;
    *ppResource = NULL;

    if (pLibrary == NULL || filePath == NULL) return OC_RESULT_INVALID_ARGS;

    // So this is how resources work in the engine... Each resource file (.png, .obj, etc.) is, optionally, associated with a corresponding
    // .ocd file. This .ocd file is located in the same location and named exactly the same, only with the ".ocd" extension appended to the
    // end. For example, "my_texture.png" can have a "my_texture.png.ocd" file with it.
    //
    // There are a few reasons for the .ocd file. The first is that it is a format that's optimized for use by the engine. The second is that
    // it provides a way of associating engine-specific metadata with the resource. Now here's an important detail - each .ocd file contains
    // _all_ of the data of the resource which means it's able to work without the original asset.
    //
    // When loading an asset, we first need to check if there's a .ocd file associated with it. If so, we need to load it up. However, there's
    // a chance the original source asset ("my_texture.png" as per the example above) has been updated. In this the .ocd file needs to be
    // updated to reflect the changes.

    // If it's an .ocd file just load it directly without looking at the original source asset.
    ocFileInfo fileInfoSrc;
    dr_bool32 hasSrc = ocGetFileInfo(pLibrary->pLoader->pFS, filePath, &fileInfoSrc) == OC_RESULT_SUCCESS;

    ocFileInfo fileInfoOCD;
    dr_bool32 hasOCD = false;
    if (!drpath_extension_equal(filePath, "ocd")) {
        char filePathOCD[OC_MAX_PATH];
        if (drpath_copy_and_append_extension(filePathOCD, sizeof(filePathOCD), filePath, "ocd")) {
            hasOCD = ocGetFileInfo(pLibrary->pLoader->pFS, filePathOCD, &fileInfoOCD) == OC_RESULT_SUCCESS;
        }
    }

    if (!hasSrc && !hasOCD) return OC_RESULT_DOES_NOT_EXIST;
    if (!hasSrc) {
        fileInfoSrc = fileInfoOCD;
    }

    // TODO: Reference count and return early based on the absolute path in fileInfoSrc.

    dr_bool32 isOCDOutOfDate = (hasSrc && hasOCD) && (fileInfoSrc.lastModifiedTime > fileInfoOCD.lastModifiedTime);

    // Here is where we actually load the asset. The file path we load from depends on whether or not we have an up-to-date OCD file.
    ocResourceType resourceType;
    ocResult result = ocResourceLoaderDetermineResourceType(pLibrary->pLoader, fileInfoSrc.absolutePath, &resourceType);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    const char* absolutePath = (!isOCDOutOfDate && hasOCD) ? fileInfoOCD.absolutePath : fileInfoSrc.absolutePath;
    switch (resourceType)
    {
        case ocResourceType_Image:
        {
            result = ocResourceLibraryLoad_Image(pLibrary, absolutePath, ppResource);
        } break;

        case ocResourceType_Scene:
        {
            result = OC_RESULT_UNSUPPORTED_RESOURCE_TYPE;
        } break;

        case ocResourceType_Unknown:
        default:
        {
            result = OC_RESULT_UNKNOWN_RESOURCE_TYPE;
        } break;
    }

    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    return OC_RESULT_SUCCESS;
}

void ocResourceLibraryUnload(ocResourceLibrary* pLibrary, ocResource* pResource)
{
    if (pLibrary == NULL || pResource == NULL) return;
    if (ocAtomicDecrement(&pResource->referenceCount) > 0) {
        return;
    }

    switch (pResource->type)
    {
        case ocResourceType_Image:
        {
            ocGraphicsDeleteImage(pLibrary->pGraphics, pResource->image.pGraphicsImage);
        } break;

        case ocResourceType_Scene:
        {
        } break;

        case ocResourceType_Unknown:
        default: break;
    }

    free(pResource);
}

ocResult ocResourceLibrarySyncOCD(ocResourceLibrary* pLibrary, const char* filePath)
{
    if (pLibrary == NULL || filePath == NULL) return OC_RESULT_INVALID_ARGS;

    // TODO: Implement me.
    return OC_RESULT_SUCCESS;
}