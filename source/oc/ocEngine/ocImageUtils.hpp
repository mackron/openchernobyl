// Copyright (C) 2016 David Reid. See included LICENSE file.

enum ocImageFormat
{
    ocImageFormat_Undefined = 0,
    ocImageFormat_R8G8B8A8  = 1,
    ocImageFormat_SRGBA8    = 2
};

///////////////////////////////////////////////////////////////////////////////
//
// Mipmap Generation
//
///////////////////////////////////////////////////////////////////////////////
struct ocMipmapInfo
{
    size_t offset;
    size_t dataSize;
    uint32_t width;
    uint32_t height;
};

// Retrieves the total number of mipmap levels for the given base image.
ocResult ocGetMipmapCount(uint32_t baseWidth, uint32_t baseHeight, uint32_t* pCount);

// Retrieves the minimum required size of a monolithic buffer that can be used to store the data of every mipmap with a particular alignment.
ocResult ocGetTotalMipmapDataSize(uint32_t baseWidth, uint32_t baseHeight, uint32_t components, uint32_t alignment, size_t* pSize);

// Generates an individual mipmap. This is just a half-sized averaged version of the base image.
ocResult ocGenerateMipmap(uint32_t baseWidth, uint32_t baseHeight, uint32_t components, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmap);

// Generates an entire chain of mipmaps, with each mipmap stored in a monolithic buffer.
ocResult ocGenerateMipmaps(uint32_t baseWidth, uint32_t baseHeight, uint32_t components, uint32_t alignment, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmaps);