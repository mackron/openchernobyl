// Copyright (C) 2016 David Reid. See included LICENSE file.

enum ocResourceType
{
    ocResourceType_Unknown,
    ocResourceType_Image,
    ocResourceType_Mesh,
    ocResourceType_Material,
    ocResourceType_Scene
};

struct ocResource
{
    ocResourceType type;
    const char* pAbsolutePath;
    uint32_t referenceCount;

    // [Internal Use Only] A single allocation for dynamically sized data.
    //
    // Format:
    //     File Name : Length + 1 (for null terminator)
    //     Padding for native alignment
    //     Resource specific dynamic data
    char _pPayload[1];
};

struct ocResourceLibrary
{
    ocResourceLoader* pLoader;
    ocGraphicsContext* pGraphics;
};

//
ocResult ocResourceLibraryInit(ocResourceLibrary* pLibrary, ocResourceLoader* pLoader, ocGraphicsContext* pGraphics);

//
void ocResourceLibraryUninit(ocResourceLibrary* pLibrary);


// Loads a resource from the file system.
ocResult ocResourceLibraryLoad(ocResourceLibrary* pLibrary, const char* filePath, ocResource** ppResource);

// Unloads a resource.
void ocResourceLibraryUnload(ocResourceLibrary* pLibrary, ocResource* pResource);