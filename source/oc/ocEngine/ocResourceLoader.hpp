// Copyright (C) 2016 David Reid. See included LICENSE file.

enum ocResourceType
{
    ocResourceType_Unknown,
    ocResourceType_Image,
    ocResourceType_Mesh,
    ocResourceType_Material,
    ocResourceType_Scene
};

struct ocImageData
{
    ocImageFormat format;
    uint32_t mipmapCount;
    ocMipmapInfo* pMipmaps;
    size_t imageDataSize;
    void* pImageData;

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

// Determines the type of resource to load.
//
// This will use the file path to determine the type, but when the path is ambiguous (such as .ocd files) it
// will look at the contents of the file.
ocResult ocResourceLoaderDetermineResourceType(ocResourceLoader* pLoader, const char* filePath, ocResourceType* pType);


// Loads an image.
ocResult ocResourceLoaderLoadImage(ocResourceLoader* pLoader, const char* filePath, ocImageData* pData);

// Unloads an image.
void ocResourceLoaderUnloadImage(ocResourceLoader* pLoader, ocImageData* pData);