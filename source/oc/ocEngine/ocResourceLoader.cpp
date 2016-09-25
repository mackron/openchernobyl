// Copyright (C) 2016 David Reid. See included LICENSE file.

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
        // stb_image
        result = ocLoadImage_STB(&file, pData);

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

    return result;
}

void ocResourceLoaderUnloadImage(ocResourceLoader* pLoader, ocImageData* pData)
{
    if (pLoader == NULL || pData == NULL) return;
    free(pData->_pPayload);
}



///////////////////////////////////////////////////////////////////////////////
//
// Meshes
//
///////////////////////////////////////////////////////////////////////////////
