// Copyright (C) 2016 David Reid. See included LICENSE file.

#define OC_OCD_FOURCC           0x2044434f /*'OCD ' LE*/
#define OC_OCD_TYPE_ID_IMAGE    0x31474d49 /*'IMG1' LE*/
#define OC_OCD_TYPE_ID_SCENE    0x314e4353 /*'SCN1' LE*/

ocResult ocResourceLoaderInit(ocResourceLoader* pLoader, ocFileSystem* pFS)
{
    if (pLoader == NULL || pFS == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pLoader);
    pLoader->pFS = pFS;

    return OC_RESULT_SUCCESS;
}

void ocResourceLoaderUninit(ocResourceLoader* pLoader)
{
    if (pLoader == NULL) return;
}


OC_PRIVATE ocResult ocResourceLoaderDetermineOCDResourceType(ocResourceLoader* pLoader, const char* filePath, ocResourceType* pType)
{
    ocAssert(pLoader != NULL);
    ocAssert(filePath != NULL);
    ocAssert(pType != NULL);

    ocFile file;
    ocResult result = ocFileOpen(pLoader->pFS, filePath, OC_READ, &file);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    size_t bytesRead;
    uint32_t ids[2];
    result = ocFileRead(&file, ids, sizeof(ids), &bytesRead);
    ocFileClose(&file);

    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    if (bytesRead != sizeof(ids) || ids[0] != OC_OCD_FOURCC) {
        return OC_RESULT_CORRUPT_FILE;
    }

    switch (ids[1])
    {
        case OC_OCD_TYPE_ID_IMAGE: *pType = ocResourceType_Image; return OC_RESULT_SUCCESS;
        case OC_OCD_TYPE_ID_SCENE: *pType = ocResourceType_Scene; return OC_RESULT_SUCCESS;
        default: return OC_RESULT_UNKNOWN_RESOURCE_TYPE;
    }
}

ocResult ocResourceLoaderDetermineResourceType(ocResourceLoader* pLoader, const char* filePath, ocResourceType* pType)
{
    if (pType == NULL) return OC_RESULT_INVALID_ARGS;
    *pType = ocResourceType_Unknown;

    if (pLoader == NULL || filePath == NULL) return OC_RESULT_INVALID_ARGS;

    const char* ext = drpath_extension(filePath);
    if (_stricmp(ext, "ocd") == 0) {
        return ocResourceLoaderDetermineOCDResourceType(pLoader, filePath, pType);
    }

    // Images.
    if (_stricmp(ext, "png") == 0 ||
        _stricmp(ext, "tga") == 0 ||
        _stricmp(ext, "jpg") == 0) {
        *pType = ocResourceType_Image;
        return OC_RESULT_SUCCESS;
    }

    // Meshes.
    if (_stricmp(ext, "obj") == 0) {
        *pType = ocResourceType_Scene;
        return OC_RESULT_SUCCESS;
    }

    return OC_RESULT_UNKNOWN_RESOURCE_TYPE;
}



///////////////////////////////////////////////////////////////////////////////
//
// Images
//
///////////////////////////////////////////////////////////////////////////////

OC_PRIVATE ocResult ocLoadImage_OCD(ocFile* pFile, ocImageData* pData)
{
    ocAssert(pFile != NULL);
    ocAssert(pData != NULL);

    // TODO: Implement me.
    (void)pFile;
    (void)pData;
    return OC_RESULT_FAILED_TO_LOAD_RESOURCE;
}


OC_PRIVATE int oc__stbi_read(void* user, char *data, int size)
{
    ocFile* pFile = (ocFile*)user;
    ocAssert(pFile != NULL);

    size_t bytesRead;
    if (ocFileRead(pFile, data, (size_t)size, &bytesRead) == OC_RESULT_SUCCESS) {
        return (int)bytesRead;
    }

    return 0;
}

OC_PRIVATE void oc__stbi_skip(void* user, int n)
{
    ocFile* pFile = (ocFile*)user;
    ocAssert(pFile != NULL);

    ocFileSeek(pFile, n, ocSeekOrigin_Current);
}

OC_PRIVATE int oc__stbi_eof(void* user)
{
    ocFile* pFile = (ocFile*)user;
    ocAssert(pFile != NULL);

    return ocAtEOF(pFile);
}

OC_PRIVATE ocResult ocLoadImage_STB(ocFile* pFile, ocImageData* pData)
{
    ocAssert(pFile != NULL);
    ocAssert(pData != NULL);

    // The engine expects upside-down images because of OpenGL and Vulkan.
    stbi_set_flip_vertically_on_load(1);

    stbi_io_callbacks cb;
    cb.read = oc__stbi_read;
    cb.skip = oc__stbi_skip;
    cb.eof  = oc__stbi_eof;

    int imageWidth;
    int imageHeight;
    stbi_uc* pImageData = stbi_load_from_callbacks(&cb, pFile, &imageWidth, &imageHeight, NULL, 4); // Always want RGBA images.
    if (pImageData == NULL) {
        return OC_RESULT_FAILED_TO_LOAD_RESOURCE;
    }

    
    // The payload is simple for single mipmap images. It's just 1 ocMipmapInfo object and then the raw image data.
    size_t mipmapInfoDataSize = sizeof(*pData->pMipmaps)*1;
    size_t imageDataSize = imageWidth*imageHeight*4;
    pData->_pPayload = malloc(mipmapInfoDataSize + imageDataSize);

    pData->format = ocImageFormat_R8G8B8A8;
    pData->mipmapCount = 1;
    pData->pMipmaps = (ocMipmapInfo*)pData->_pPayload;
    pData->imageDataSize = imageDataSize;
    pData->pImageData = ocOffsetPtr(pData->_pPayload, mipmapInfoDataSize);
    memcpy(pData->pImageData, pImageData, imageDataSize);

    // Only one mipmap.
    pData->pMipmaps[0].offset = 0;
    pData->pMipmaps[0].dataSize = imageDataSize;
    pData->pMipmaps[0].width = (uint32_t)imageWidth;
    pData->pMipmaps[0].height = (uint32_t)imageHeight;

    stbi_image_free(pImageData);
    return OC_RESULT_SUCCESS;
}

ocResult ocResourceLoaderLoadImage(ocResourceLoader* pLoader, const char* filePath, ocImageData* pData)
{
    if (pData == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pData);

    if (pLoader == NULL) return OC_RESULT_INVALID_ARGS;

    // Try opening the file to begin with.
    ocFile file;
    ocResult result = ocFileOpen(pLoader->pFS, filePath, OC_READ, &file);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // We use a trial and error system for loading different file formats. If one fails, we just fall through to the
    // next sub-loader and try again. The exception is .ocd files which is the native file format for the engine. When
    // a file with this extension is specified, it will _not_ fall through to the next sub-loaders.
    if (drpath_extension_equal(filePath, "ocd")) {
        result = ocLoadImage_OCD(&file, pData);
    } else {
        result = OC_RESULT_FAILED_TO_LOAD_RESOURCE;

        // stb_image
        if (result != OC_RESULT_SUCCESS) {
            result = ocLoadImage_STB(&file, pData);
        }

#if 0
        // PCX
        if (result != OC_RESULT_SUCCESS) {
            result = ocLoadImage_PCX(&file, pData);
        }

        // KTX
        if (result != OC_RESULT_SUCCESS) {
            result = ocLoadImage_KTX(&file, pData);
        }

        // DDS
        if (result != OC_RESULT_SUCCESS) {
            result = ocLoadImage_DDS(&file, pData);
        }
#endif
    }

    ocFileClose(&file);
    return result;
}

void ocResourceLoaderUnloadImage(ocResourceLoader* pLoader, ocImageData* pData)
{
    if (pLoader == NULL || pData == NULL) return;
    free(pData->_pPayload);
}



///////////////////////////////////////////////////////////////////////////////
//
// Scenes
//
///////////////////////////////////////////////////////////////////////////////

OC_PRIVATE ocResult ocLoadScene_OCD(ocFile* pFile, ocSceneData* pData)
{
    ocAssert(pFile != NULL);
    ocAssert(pData != NULL);

    // TODO: Implement me.
    (void)pFile;
    (void)pData;
    return OC_RESULT_FAILED_TO_LOAD_RESOURCE;
}


OC_PRIVATE size_t oc__drobj_read(void* userData, void* bufferOut, size_t bytesToRead)
{
    size_t bytesRead;
    ocResult result = ocFileRead((ocFile*)userData, bufferOut, bytesToRead, &bytesRead);
    if (result != OC_RESULT_SUCCESS) {
        return 0;
    }

    return bytesRead;
}

OC_PRIVATE dr_bool32 oc__drobj_seek_to_start(void* userData)
{
    return ocFileSeek((ocFile*)userData, 0, ocSeekOrigin_Start) == OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocLoadScene_OBJ(ocFile* pFile, ocSceneData* pData)
{
    ocAssert(pFile != NULL);
    ocAssert(pData != NULL);

    drobj* obj = drobj_load(oc__drobj_read, oc__drobj_seek_to_start, pFile);
    if (obj == NULL) {
        return OC_RESULT_FAILED_TO_LOAD_RESOURCE;
    }

    // The first thing to do is calculate the size of the raw data. 

    drobj_delete(obj);

    // TODO: Implement me.
    (void)pFile;
    (void)pData;
    return OC_RESULT_FAILED_TO_LOAD_RESOURCE;
}

ocResult ocResourceLoaderLoadScene(ocResourceLoader* pLoader, const char* filePath, ocSceneData* pData)
{
    if (pData == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pData);

    if (pLoader == NULL || filePath == NULL) return OC_RESULT_INVALID_ARGS;

    // Try opening the file to begin with.
    ocFile file;
    ocResult result = ocFileOpen(pLoader->pFS, filePath, OC_READ, &file);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // We use a trial and error system for loading different file formats. If one fails, we just fall through to the
    // next sub-loader and try again. The exception is .ocd files which is the native file format for the engine. When
    // a file with this extension is specified, it will _not_ fall through to the next sub-loaders.
    if (drpath_extension_equal(filePath, "ocd")) {
        result = ocLoadScene_OCD(&file, pData);
    } else {
        result = OC_RESULT_FAILED_TO_LOAD_RESOURCE;

        // OBJ.
        if (result != OC_RESULT_SUCCESS && drpath_extension_equal(filePath, "obj")) {
            result = ocLoadScene_OBJ(&file, pData);
        }

#if 0
        // glTF
        if (result != OC_RESULT_SUCCESS) {
            result = ocLoadScene_glTF(&file, pData);
        }

        // Assimp, maybe?
        if (result != OC_RESULT_SUCCESS) {
            result = ocLoadScene_Assimp(&file, pData);
        }
#endif
    }

    ocFileClose(&file);
    return result;
}

void ocResourceLoaderUnloadScene(ocResourceLoader* pLoader, ocSceneData* pData)
{
    if (pLoader == NULL || pData == NULL) return;
    free(pData->_pPayload);
}

