// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocResourceLibraryInit(ocResourceLibrary* pLibrary, ocResourceLoader* pLoader, ocGraphicsContext* pGraphics)
{
    if (pLibrary == NULL || pLoader == NULL || pGraphics == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pLibrary);
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

ocResult ocResourceLibraryLoad(ocResourceLibrary* pLibrary, const char* filePath, ocResource** ppResource)
{
    if (ppResource == NULL) return OC_RESULT_INVALID_ARGS;
    *ppResource = NULL;

    if (pLibrary == NULL || filePath == NULL) return OC_RESULT_INVALID_ARGS;

    // TODO: Add support for loading an associated .ocd file. Look at the last modified time: if newer, load that one. If older, ignore it
    //       and continue loading the source asset. Might want to set a flag indicating that the .ocd file is out of date so that a higher
    //       level API can inspect that flag and re-create the OCD file.
    //
    // TODO: Check if the resource has already been loaded and simply increment the reference count.

    // For the moment just use trial and error.
    //
    // TODO: Optimize this for .ocd files. Just read the first bytes and inspect the relevant FourCC codes to determine the type.

    // IDEA: ocResourceLoaderDetermineResourceType(pLoader, absolutePath, &file, &type);
    //       Maybe change ocResourceLoaderLoad*() to take an already open file handle? Could also just have ocResourceLoaderLoadImageFromFile().

    char absolutePath[OC_MAX_PATH];
    ocResult result = ocFindAbsoluteFilePath(pLibrary->pLoader->pFS, filePath, absolutePath, sizeof(absolutePath));
    if (result != OC_RESULT_SUCCESS) {
        return result;  // The file probably doesn't exist.
    }

    ocResourceType resourceType;
    result = ocResourceLoaderDetermineResourceType(pLibrary->pLoader, absolutePath, &resourceType);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    switch (resourceType)
    {
        case ocResourceType_Image:
        {
            ocImageData data;
            result = ocResourceLoaderLoadImage(pLibrary->pLoader, absolutePath, &data);
            if (result == OC_RESULT_SUCCESS) {
                bool freeImageData = false;
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
                    // Use pre-determined mipmaps.
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
        } break;

        case ocResourceType_Mesh:
        {
        } break;

        case ocResourceType_Unknown:
        default: break;
    }



    // If we get here it means we were not able to load the resource as any type. It likely means the file does not exist or it's just
    // an unsupported format.
    return OC_RESULT_FAILED_TO_LOAD_RESOURCE;
}

void ocResourceLibraryUnload(ocResourceLibrary* pLibrary, ocResource* pResource)
{
    if (pLibrary == NULL || pResource == NULL) return;

    // TODO: Decrement reference count.

    free(pResource);
}