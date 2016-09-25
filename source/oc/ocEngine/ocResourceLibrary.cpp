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

    // Images.
    {
        ocImageData data;
        ocResult result = ocResourceLoaderLoadImage(pLibrary->pLoader, filePath, &data);
        if (result == OC_RESULT_SUCCESS) {
            // TODO: Upload the image to the graphics system.

            ocResourceLoaderUnloadImage(pLibrary->pLoader, &data);
            return result;
        }
    }
    
    // Meshes.
}

void ocResourceLibraryUnload(ocResourceLibrary* pLibrary, ocResource* pResource)
{
    if (pLibrary == NULL || pResource == NULL) return;

    // TODO: Decrement reference count.

    free(pResource);
}