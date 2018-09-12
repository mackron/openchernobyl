// Copyright (C) 2018 David Reid. See included LICENSE file.

ocUInt32 ocImageFormatComponentCount(ocImageFormat format)
{
    ocUInt32 table[] = {
        0,  // ocImageFormat_Undefined
        4,  // ocImageFormat_R8G8B8A8
        4   // ocImageFormat_SRGBA8
    };

    return table[format];
}

ocUInt32 ocImageFormatBytesPerPixel(ocImageFormat format)
{
    ocUInt32 table[] = {
        0,  // ocImageFormat_Undefined
        4,  // ocImageFormat_R8G8B8A8
        4   // ocImageFormat_SRGBA8
    };

    return table[format];
}



ocResult ocGetMipmapCount(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32* pCount)
{
    if (pCount == NULL) return OC_INVALID_ARGS;
    *pCount = 0;

    if (baseWidth == 0 || baseHeight == 0) return OC_INVALID_ARGS;

    *pCount = 1 + (unsigned int)log2f((float)ocMax(baseWidth, baseHeight));
    return OC_SUCCESS;
}

ocResult ocGetTotalMipmapDataSize(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32 components, ocUInt32 alignment, ocSizeT* pSize)
{
    if (pSize == NULL) return OC_INVALID_ARGS;
    *pSize = 0;

    if (baseWidth == 0 || baseHeight == 0 || components == 0) return OC_INVALID_ARGS;
    if (alignment == 0) alignment = 1;

    ocSizeT totalSize = 0;

    ocSizeT mipmapSizeX = (ocSizeT)baseWidth;
    ocSizeT mipmapSizeY = (ocSizeT)baseHeight;
    for (;;) {
        ocSizeT mipmapDataSize = mipmapSizeX * mipmapSizeY * components;

        totalSize += mipmapDataSize;
        totalSize = ocAlign(totalSize, alignment);  // Padding for alignment. Also added to the end to ensure the total size of buffer is also aligned.

        // If it was a 1x1 image we're done.
        if (mipmapDataSize == components) {
            break;
        }

        if (mipmapSizeX > 1) mipmapSizeX >>= 1;
        if (mipmapSizeY > 1) mipmapSizeY >>= 1;
    }

    *pSize = totalSize;
    return OC_SUCCESS;
}

ocResult ocGenerateMipmap(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32 components, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmap)
{
    if (baseWidth == 0 || baseHeight == 0 || components == 0 || pBaseData == NULL || pDataOut == NULL) return OC_INVALID_ARGS;

    // TODO: Optimized branch for 4 components.

    // The y+1 and x+1 statements are used to handle cases when the base image is not evenly divisible by 2, such as when it's 3x3 going to 1x1. It avoids
    // going out of bounds.
    uint8_t* pRunningDataOut = (uint8_t*)pDataOut;
    for (ocUInt32 y = 0; y+1 < baseHeight; y += 2) {
        for (ocUInt32 x = 0; x+1 < baseWidth; x += 2) {
            uint8_t* pBaseTexel = (uint8_t*)pBaseData + (((baseWidth * y) + x) * components);
            for (ocUInt32 c = 0; c < components; ++c) {
                ocUInt32 c00 = pBaseTexel[c + 0];
                ocUInt32 c01 = pBaseTexel[c + components];
                ocUInt32 c10 = pBaseTexel[c + baseWidth*components];
                ocUInt32 c11 = pBaseTexel[c + baseWidth*components + components];
                pRunningDataOut[c] = (uint8_t)((c00 + c01 + c10 + c11) >> 2);
            }

            pRunningDataOut += components;
        }

        
    }

    if (pMipmap != NULL) {
        pMipmap->width = baseWidth >> 1;
        pMipmap->height = baseHeight >> 1;
        pMipmap->dataSize = pMipmap->width * pMipmap->height * components;
        pMipmap->dataOffset = 0;
    }

    return OC_SUCCESS;
}

ocResult ocGenerateMipmaps(ocUInt32 baseWidth, ocUInt32 baseHeight, ocUInt32 components, ocUInt32 alignment, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmaps)
{
    if (baseWidth == 0 || baseHeight == 0 || components == 0 || pBaseData == NULL || pDataOut == NULL) return OC_INVALID_ARGS;
    if (alignment == 0) alignment = 1;

    ocSizeT runningOffset = 0;

    ocUInt32 prevWidth = baseWidth;
    ocUInt32 prevHeight = baseHeight;
    const void* pPrevData = pBaseData;

    ocUInt32 iMipmap = 0;
    for (;;) {
        ocUInt32 prevDataSize = prevWidth * prevHeight * components;

        void* pDstDataOut = ocOffsetPtr(pDataOut, runningOffset);
        memcpy(pDstDataOut, pPrevData, prevDataSize);

        if (pMipmaps != NULL) {
            pMipmaps[iMipmap].dataOffset = runningOffset;
            pMipmaps[iMipmap].dataSize = prevDataSize;
            pMipmaps[iMipmap].width = prevWidth;
            pMipmaps[iMipmap].height = prevHeight;
        }


        // If it was a 1x1 image we're done.
        if (prevDataSize == components) {
            break;
        }

        runningOffset += prevDataSize;
        runningOffset = ocAlign(runningOffset, alignment);

        ocUInt32 dstSizeX = ocMax(1, prevWidth >> 1);
        ocUInt32 dstSizeY = ocMax(1, prevHeight >> 1);
        pDstDataOut = ocOffsetPtr(pDataOut, runningOffset);
        ocGenerateMipmap(prevWidth, prevHeight, components, pPrevData, pDstDataOut, NULL);

        // The new mipmap becomes the new base.
        prevWidth = dstSizeX;
        prevHeight = dstSizeY;
        pPrevData = pDstDataOut;
        iMipmap += 1;
    }

    return OC_SUCCESS;
}