// Copyright (C) 2018 David Reid. See included LICENSE file.

enum ocImageFormat
{
    ocImageFormat_Undefined = 0,
    ocImageFormat_R8G8B8A8,
    ocImageFormat_SRGBA8,
    ocImageFormat_R16G16B16A16F
};

ocUInt32 ocImageFormatComponentCount(ocImageFormat format);
ocUInt32 ocImageFormatBytesPerPixel(ocImageFormat format);


///////////////////////////////////////////////////////////////////////////////
//
// Mipmap Generation
//
///////////////////////////////////////////////////////////////////////////////

// This structure must map to the OCD file format.
struct ocMipmapInfo
{
    ocUInt64 dataOffset;
    ocUInt64 dataSize;
    ocUInt32 width;
    ocUInt32 height;
};

// Retrieves the total number of mipmap levels for the given base image.
ocResult ocGetMipmapCount(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32* pCount);

// Retrieves the minimum required size of a monolithic buffer that can be used to store the data of every mipmap with a particular alignment.
ocResult ocGetTotalMipmapDataSize(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32 components, ocUInt32 alignment, ocSizeT* pSize);

// Generates an individual mipmap. This is just a half-sized averaged version of the base image.
ocResult ocGenerateMipmap(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32 components, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmap);

// Generates an entire chain of mipmaps, with each mipmap stored in a monolithic buffer.
ocResult ocGenerateMipmaps(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32 components, ocUInt32 alignment, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmaps);