// Copyright (C) 2018 David Reid. See included LICENSE file.

OC_PRIVATE ocBool32 ocCheckOCDHeader(const void* pData, ocUInt64 dataSize, ocUInt32 expectedType)
{
    if (pData == NULL || dataSize < 8) {
        return MAL_FALSE;
    }

    ocUInt32 fourcc = *(ocUInt32*)pData;
    if (fourcc != OC_OCD_FOURCC) {
        return MAL_FALSE;
    }

    ocUInt32 type = *((ocUInt32*)pData + 1);
    if (type != expectedType) {
        return MAL_FALSE;
    }
    
    return MAL_TRUE;
}

// Free the returned pointer with ocFree().
OC_PRIVATE ocResult ocMallocAndReadEntireStreamReader(ocStreamReader* pReader, void** ppDataOut, ocUInt64* pSizeOut)
{
    ocAssert(pReader != NULL);
    ocAssert(pSizeOut != NULL);
    ocAssert(ppDataOut != NULL);
    ocAssert(pSizeOut != NULL);
    
    *ppDataOut = NULL;
    *pSizeOut = 0;

    ocUInt64 fileSize;
    ocResult result = ocStreamReaderSize(pReader, &fileSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    if (fileSize > SIZE_MAX) {
        return OC_TOO_LARGE; // OCD file is too big to fit into memory.
    }

    void* pDataOut = (ocUInt8*)ocMalloc((ocSizeT)fileSize);
    if (pDataOut == NULL) {
        return OC_OUT_OF_MEMORY;
    }

    result = ocStreamReaderRead(pReader, pDataOut, (ocSizeT)fileSize, NULL);
    if (result != OC_SUCCESS) {
        ocFree(pDataOut);
        return result;
    }

    *ppDataOut = pDataOut;
    *pSizeOut = fileSize;
    return OC_SUCCESS;
}


ocResult ocResourceLoaderInit(ocFileSystem* pFS, ocResourceLoader* pLoader)
{
    if (pLoader == NULL || pFS == NULL) return OC_INVALID_ARGS;

    ocZeroObject(pLoader);
    pLoader->pFS = pFS;

    return OC_SUCCESS;
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
    if (result != OC_SUCCESS) {
        return result;
    }

    size_t bytesRead;
    uint32_t ids[2];
    result = ocFileRead(&file, ids, sizeof(ids), &bytesRead);
    ocFileClose(&file);

    if (result != OC_SUCCESS) {
        return result;
    }

    if (bytesRead != sizeof(ids) || ids[0] != OC_OCD_FOURCC) {
        return OC_CORRUPT_FILE;
    }

    switch (ids[1])
    {
        case OC_OCD_TYPE_ID_IMAGE: *pType = ocResourceType_Image; return OC_SUCCESS;
        case OC_OCD_TYPE_ID_SCENE: *pType = ocResourceType_Scene; return OC_SUCCESS;
        default: return OC_UNKNOWN_RESOURCE_TYPE;
    }
}

ocResult ocResourceLoaderDetermineResourceType(ocResourceLoader* pLoader, const char* filePath, ocResourceType* pType)
{
    if (pType == NULL) return OC_INVALID_ARGS;
    *pType = ocResourceType_Unknown;

    if (pLoader == NULL || filePath == NULL) return OC_INVALID_ARGS;

    const char* ext = drpath_extension(filePath);
    if (_stricmp(ext, "ocd") == 0) {
        return ocResourceLoaderDetermineOCDResourceType(pLoader, filePath, pType);
    }

    // Images.
    if (_stricmp(ext, "png") == 0 ||
        _stricmp(ext, "tga") == 0 ||
        _stricmp(ext, "jpg") == 0) {
        *pType = ocResourceType_Image;
        return OC_SUCCESS;
    }

#ifdef OC_ENABLE_PCX
    if (_stricmp(ext, "pcx") == 0) {
        *pType = ocResourceType_Image;
        return OC_SUCCESS;
    }
#endif
#ifdef OC_ENABLE_KTX
    if (_stricmp(ext, "ktx") == 0) {
        *pType = ocResourceType_Image;
        return OC_SUCCESS;
    }
#endif
#ifdef OC_ENABLE_DDS
    if (_stricmp(ext, "dds") == 0) {
        *pType = ocResourceType_Image;
        return OC_SUCCESS;
    }
#endif


    // Meshes.
    if (_stricmp(ext, "obj") == 0) {
        *pType = ocResourceType_Scene;
        return OC_SUCCESS;
    }

    return OC_UNKNOWN_RESOURCE_TYPE;
}



///////////////////////////////////////////////////////////////////////////////
//
// Images
//
///////////////////////////////////////////////////////////////////////////////

OC_PRIVATE ocResult ocConvertToOCD_SimpleImage(ocImageFormat format, ocUInt32 width, ocUInt32 height, const void* pImageData, ocStreamWriter* pOCDWriter)
{
    ocAssert(pOCDWriter != NULL);

    ocOCDImageBuilder builder;
    ocResult result = ocOCDImageBuilderInit(format, width, height, pImageData, &builder);
    if (result != OC_SUCCESS) {
        return result;
    }

    result = ocOCDImageBuilderRender(&builder, pOCDWriter);

    ocOCDImageBuilderUninit(&builder);
    return result;
}


OC_PRIVATE ocResult ocLoadImage_OCD(ocStreamReader* pReader, ocImageData* pData)
{
    ocAssert(pReader != NULL);
    ocAssert(pData != NULL);

    ocUInt64 fileSize;
    ocResult result = ocMallocAndReadEntireStreamReader(pReader, (void**)&pData->pPayload, &fileSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    if (!ocCheckOCDHeader(pData->pPayload, fileSize, OC_OCD_TYPE_ID_IMAGE)) {
        ocFree(pData->pPayload);
        return OC_CORRUPT_FILE;
    }

    pData->format        = (ocImageFormat)(*(ocUInt32*)(pData->pPayload + OC_OCD_HEADER_SIZE + 0));
    pData->mipmapCount   =                 *(ocUInt32*)(pData->pPayload + OC_OCD_HEADER_SIZE + 4);
    pData->pMipmaps      =              (ocMipmapInfo*)(pData->pPayload + OC_OCD_HEADER_SIZE + 8);
    pData->imageDataSize =                 *(ocUInt64*)(pData->pPayload + OC_OCD_HEADER_SIZE + 8 + (sizeof(ocMipmapInfo)*pData->mipmapCount));
    pData->pImageData    =                             (pData->pPayload + OC_OCD_HEADER_SIZE + 8 + (sizeof(ocMipmapInfo)*pData->mipmapCount) + 8);

    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocLoadImage_Raw(ocImageFormat format, ocUInt32 width, ocUInt32 height, const void* pImageData, ocImageData* pData)
{
    // We convert the image data to an OCD file, and then load the image via the OCD loading pipeline. Inefficient, but keeps all image loading
    // on the same code path.
    void* pDataOCD;
    ocSizeT dataSizeOCD;
    ocStreamWriter writerOCD;
    ocResult result = ocStreamWriterInit(&pDataOCD, &dataSizeOCD, &writerOCD);
    if (result != OC_SUCCESS) {
        return result;
    }

    result = ocConvertToOCD_SimpleImage(format, width, height, pImageData, &writerOCD);
    ocStreamWriterUninit(&writerOCD);   // <-- Don't need this anymore. Data will be in pDataOCD, which needs to be ocFree()'d by us.

    if (result != OC_SUCCESS) {
        ocFree(pDataOCD);
        return result;
    }


    ocStreamReader readerOCD;
    result = ocStreamReaderInit(pDataOCD, dataSizeOCD, &readerOCD);
    if (result != OC_SUCCESS) {
        ocFree(pDataOCD);
        return result;
    }

    result = ocLoadImage_OCD(&readerOCD, pData);
    
    ocStreamReaderUninit(&readerOCD);
    ocFree(pDataOCD);
    return result;
}


OC_PRIVATE int oc__stbi_read(void* user, char *data, int size)
{
    ocStreamReader* pReader = (ocStreamReader*)user;
    ocAssert(pReader != NULL);

    size_t bytesRead;
    if (ocStreamReaderRead(pReader, data, (size_t)size, &bytesRead) == OC_SUCCESS) {
        return (int)bytesRead;
    }

    return 0;
}

OC_PRIVATE void oc__stbi_skip(void* user, int n)
{
    ocStreamReader* pReader = (ocStreamReader*)user;
    ocAssert(pReader != NULL);

    ocStreamReaderSeek(pReader, n, ocSeekOrigin_Current);
}

OC_PRIVATE int oc__stbi_eof(void* user)
{
    ocStreamReader* pReader = (ocStreamReader*)user;
    ocAssert(pReader != NULL);

    return ocStreamReaderAtEnd(pReader);
}

OC_PRIVATE ocResult ocLoadImage_STB(ocStreamReader* pReader, ocImageData* pData)
{
    ocAssert(pReader != NULL);
    ocAssert(pData != NULL);

    // The engine expects upside-down images because of OpenGL and Vulkan.
    stbi_set_flip_vertically_on_load(1);

    stbi_io_callbacks cb;
    cb.read = oc__stbi_read;
    cb.skip = oc__stbi_skip;
    cb.eof  = oc__stbi_eof;

    int imageWidth;
    int imageHeight;
    stbi_uc* pImageData = stbi_load_from_callbacks(&cb, pReader, &imageWidth, &imageHeight, NULL, 4); // Always want RGBA images.
    if (pImageData == NULL) {
        return OC_FAILED_TO_LOAD_RESOURCE;
    }

    ocResult result = ocLoadImage_Raw(ocImageFormat_R8G8B8A8, imageWidth, imageHeight, pImageData, pData);
    stbi_image_free(pImageData);

    return result;
}

ocResult ocResourceLoaderLoadImage(ocResourceLoader* pLoader, const char* filePath, ocImageData* pData)
{
    if (pData == NULL) return OC_INVALID_ARGS;
    ocZeroObject(pData);

    if (pLoader == NULL) return OC_INVALID_ARGS;

    // Try opening the file to begin with.
    ocFile file;
    ocResult result = ocFileOpen(pLoader->pFS, filePath, OC_READ, &file);
    if (result != OC_SUCCESS) {
        return result;
    }

    ocStreamReader reader;
    result = ocStreamReaderInit(&file, &reader);
    if (result != OC_SUCCESS) {
        ocFileClose(&file);
        return result;
    }


    // We use a trial and error system for loading different file formats. If one fails, we just fall through to the
    // next sub-loader and try again. The exception is .ocd files which is the native file format for the engine. When
    // a file with this extension is specified, it will _not_ fall through to the next sub-loaders.
    if (drpath_extension_equal(filePath, "ocd")) {
        result = ocLoadImage_OCD(&reader, pData);
    } else {
        result = OC_FAILED_TO_LOAD_RESOURCE;

        // stb_image
        if (result != OC_SUCCESS) {
            result = ocLoadImage_STB(&reader, pData);
        }

#ifdef OC_ENABLE_PCX
        if (result != OC_SUCCESS) {
            result = ocLoadImage_PCX(&reader, pData);
        }
#endif
#ifdef OC_ENABLE_KTX
        if (result != OC_SUCCESS) {
            result = ocLoadImage_KTX(&reader, pData);
        }
#endif
#ifdef OC_ENABLE_DDS
        if (result != OC_SUCCESS) {
            result = ocLoadImage_DDS(&reader, pData);
        }
#endif
    }

    ocStreamReaderUninit(&reader);
    ocFileClose(&file);
    return result;
}

void ocResourceLoaderUnloadImage(ocResourceLoader* pLoader, ocImageData* pData)
{
    if (pLoader == NULL || pData == NULL) return;
    ocFree(pData->pPayload);
}



///////////////////////////////////////////////////////////////////////////////
//
// Scenes
//
///////////////////////////////////////////////////////////////////////////////

OC_PRIVATE ocResult ocLoadScene_OCD(ocStreamReader* pReader, ocSceneData* pData)
{
    ocAssert(pReader != NULL);
    ocAssert(pData != NULL);

    // Loading an OCD file is very simple because the file format nicely maps to our data structures.
    ocUInt64 fileSize;
    ocResult result = ocMallocAndReadEntireStreamReader(pReader, (void**)&pData->pPayload, &fileSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    if (!ocCheckOCDHeader(pData->pPayload, fileSize, OC_OCD_TYPE_ID_SCENE)) {
        ocFree(pData->pPayload);
        return OC_CORRUPT_FILE;
    }


    // Retrieve the subresource and object counts and offsets for convenience.
    pData->subresourceCount = *(ocUInt32*)(pData->pPayload + OC_OCD_HEADER_SIZE + 0);
    pData->objectCount      = *(ocUInt32*)(pData->pPayload + OC_OCD_HEADER_SIZE + 4);

    ocUInt64 subresourcesOffset = *(ocUInt64*)(pData->pPayload + OC_OCD_HEADER_SIZE + 8);
    ocUInt64 objectsOffset      = *(ocUInt64*)(pData->pPayload + OC_OCD_HEADER_SIZE + 16);

    pData->pSubresources = (ocSceneSubresource*)(pData->pPayload + subresourcesOffset);
    pData->pObjects      =      (ocSceneObject*)(pData->pPayload + objectsOffset);
    
    return OC_SUCCESS;
}


OC_PRIVATE size_t oc__drobj_read(void* userData, void* bufferOut, size_t bytesToRead)
{
    size_t bytesRead;
    ocResult result = ocStreamReaderRead((ocStreamReader*)userData, bufferOut, bytesToRead, &bytesRead);
    if (result != OC_SUCCESS) {
        return 0;
    }

    return bytesRead;
}

OC_PRIVATE ocBool32 oc__drobj_seek_to_start(void* userData)
{
    return ocStreamReaderSeek((ocStreamReader*)userData, 0, ocSeekOrigin_Start) == OC_SUCCESS;
}

OC_PRIVATE ocResult ocConvertToOCD_OBJ(ocStreamReader* pOBJReader, ocStreamWriter* pOCDWriter)
{
    ocAssert(pOBJReader != NULL);
    ocAssert(pOCDWriter != NULL);

    drobj* obj = drobj_load(oc__drobj_read, oc__drobj_seek_to_start, pOBJReader);
    if (obj == NULL) {
        return OC_FAILED_TO_LOAD_RESOURCE;
    }

    // We use a scene builder to convert OBJ to OCD.
    ocOCDSceneBuilder builder;
    ocResult result = ocOCDSceneBuilderInit(&builder);
    if (result != OC_SUCCESS) {
        drobj_delete(obj);
        return result;
    }

    result = ocOCDSceneBuilderBeginObject(&builder, "mesh", glm::vec3(0, 0, 0), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1));
    if (result != OC_SUCCESS) {
        goto done;
    }
    {
        // The object has a single mesh component, with a group for each material.
        result = ocOCDSceneBuilderBeginMeshComponent(&builder);
        if (result != OC_SUCCESS) {
            goto done;
        }
        {
            for (ocUInt32 iMaterial = 0; iMaterial < obj->materialCount; ++iMaterial) {
                drobj_material* pMaterial = &obj->pMaterials[iMaterial];
                
                ocUInt32 vertexCount;
                float* pVertexData;
                ocUInt32 indexCount;
                ocUInt32* pIndexData;
                drobj_interleave_p3t2n3_material(obj, iMaterial, &vertexCount, &pVertexData, &indexCount, &pIndexData);

                // TODO: With the material, might want to first find the full library/name path.

                result = ocOCDSceneBuilderMeshComponentAddGroup(&builder, pMaterial->name, ocGraphicsPrimitiveType_Triangle, ocGraphicsVertexFormat_P3T2N3, vertexCount, pVertexData, indexCount, pIndexData);
                if (result != OC_SUCCESS) {
                    drobj_free(pVertexData);
                    drobj_free(pIndexData);
                    goto done;
                }

                drobj_free(pVertexData);
                drobj_free(pIndexData);
            }
        }
        result = ocOCDSceneBuilderEndMeshComponent(&builder);
        if (result != OC_SUCCESS) {
            goto done;
        }
    }
    result = ocOCDSceneBuilderEndObject(&builder);
    if (result != OC_SUCCESS) {
        goto done;
    }

    result = ocOCDSceneBuilderRender(&builder, pOCDWriter);
    if (result != OC_SUCCESS) {
        goto done;
    }

    result = OC_SUCCESS;

done:
    ocOCDSceneBuilderUninit(&builder);
    drobj_delete(obj);
    return result;
}

OC_PRIVATE ocResult ocLoadScene_OBJ(ocStreamReader* pReader, ocSceneData* pData)
{
    ocAssert(pReader != NULL);
    ocAssert(pData != NULL);

    // Loading an OBJ file is a bit unintuitive. We actually convert it to an in-memory OCD file first, and then simply load the resulting OCD. The
    // reason for this design is that loading scene resources is complicated, and I want all types to be loaded through the same code path for safety
    // and maintainability (that code path being the OCD loading routine). In addition, all scene resource types need to be convertible to OCD anyway
    // which makes this design choice good for the sake of code reuse.

    // A stream writer is the where the OCD file is output.
    void* pDataOCD;
    ocSizeT dataSizeOCD;
    ocStreamWriter writerOCD;
    ocResult result = ocStreamWriterInit(&pDataOCD, &dataSizeOCD, &writerOCD);
    if (result != OC_SUCCESS) {
        return result;
    }

    // Convert the OBJ file to an OCD file. pReader is the OBJ file, writerOCD is the OCD file.
    result = ocConvertToOCD_OBJ(pReader, &writerOCD);
    if (result != OC_SUCCESS) {
        ocStreamWriterUninit(&writerOCD);
        ocFree(pDataOCD);
        return result;
    }


    // The stream writer is no longer needed. At this point, pDataOCD contains the raw OCD data (which needs to be ocFree()'d by us).
    ocStreamWriterUninit(&writerOCD);


    // Now we just load the OCD file like normal.
    ocStreamReader readerOCD;
    result = ocStreamReaderInit(pDataOCD, dataSizeOCD, &readerOCD);
    if (result != OC_SUCCESS) {
        ocFree(pDataOCD);
        return result;
    }

    result = ocLoadScene_OCD(&readerOCD, pData);
    
    ocStreamReaderUninit(&readerOCD);
    ocFree(pDataOCD);
    return result;
}


ocResult ocResourceLoaderLoadScene(ocResourceLoader* pLoader, const char* filePath, ocSceneData* pData)
{
    if (pData == NULL) return OC_INVALID_ARGS;
    ocZeroObject(pData);

    if (pLoader == NULL || filePath == NULL) return OC_INVALID_ARGS;

    // Try opening the file to begin with.
    ocFile file;
    ocResult result = ocFileOpen(pLoader->pFS, filePath, OC_READ, &file);
    if (result != OC_SUCCESS) {
        return result;
    }

    ocStreamReader reader;
    result = ocStreamReaderInit(&file, &reader);
    if (result != OC_SUCCESS) {
        ocFileClose(&file);
        return result;
    }

    // We use a trial and error system for loading different file formats. If one fails, we just fall through to the
    // next sub-loader and try again. The exception is .ocd files which is the native file format for the engine. When
    // a file with this extension is specified, it will _not_ fall through to the next sub-loaders.
    if (drpath_extension_equal(filePath, "ocd")) {
        result = ocLoadScene_OCD(&reader, pData);
    } else {
        result = OC_FAILED_TO_LOAD_RESOURCE;

        // OBJ.
        if (result != OC_SUCCESS && drpath_extension_equal(filePath, "obj")) {
            result = ocLoadScene_OBJ(&reader, pData);
        }

#if 0
        // glTF
        if (result != OC_SUCCESS) {
            result = ocLoadScene_glTF(&reader, pData);
        }

        // Assimp, maybe?
        if (result != OC_SUCCESS) {
            result = ocLoadScene_Assimp(&reader, pData);
        }
#endif
    }

    ocStreamReaderUninit(&reader);
    ocFileClose(&file);
    return result;
}

void ocResourceLoaderUnloadScene(ocResourceLoader* pLoader, ocSceneData* pData)
{
    if (pLoader == NULL || pData == NULL) return;
    ocFree(pData->pPayload);
}

