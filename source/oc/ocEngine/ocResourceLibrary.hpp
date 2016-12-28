// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocResource
{
    ocResourceType type;
    const char* pAbsolutePath;
    uint32_t referenceCount;

    union
    {
        struct
        {
            ocGraphicsImage* pGraphicsImage;
        } image;
    };

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
ocResult ocResourceLibraryInit(ocResourceLoader* pLoader, ocGraphicsContext* pGraphics, ocResourceLibrary* pLibrary);

//
void ocResourceLibraryUninit(ocResourceLibrary* pLibrary);


// Loads a resource from the file system.
ocResult ocResourceLibraryLoad(ocResourceLibrary* pLibrary, const char* filePath, ocResource** ppResource);

// Unloads a resource.
void ocResourceLibraryUnload(ocResourceLibrary* pLibrary, ocResource* pResource);

// Synchronizes or creates the .ocd file associated with a resource.
ocResult ocResourceLibrarySyncOCD(ocResourceLibrary* pLibrary, const char* filePath);