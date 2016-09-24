// Copyright (C) 2016 David Reid. See included LICENSE file.

enum ocImageFormat
{
    ocImageFormat_Undefined = 0,
    ocImageFormat_R8G8B8    = 1,
    ocImageFormat_SRGB8     = 2,
    ocImageFormat_R8G8B8A8  = 3,
    ocImageFormat_SRGBA8    = 4
};

struct ocImageData
{
    ocImageFormat format;
    uint32_t mipmapCount;
    ocMipmapInfo* pMipmaps;
    size_t imageDataSize;
    const void* pImageData;

    // [Internal Use Only] Dynamically allocated data as a single allocation.
    //
    // Format:
    //   sizeof(ocMipmapInfo) * mipmapCount
    //   Raw image data of imageDataSize bytes
    void* _pPayload;
};

struct ocResourceLoader
{
    ocFileSystem* pFS;
};

//
ocResult ocResourceLoaderInit(ocResourceLoader* pLoader, ocFileSystem* pFS);

//
void ocResourceLoaderUninit(ocResourceLoader* pLoader);


// Loads an image.
ocResult ocResouceLoaderLoadImage(ocResourceLoader* pLoader, const char* filePath, ocImageData* pData);

// Unloads an image.
void ocResourceLoaderUnloadImage(ocResourceLoader* pLoader, ocImageData* pData);