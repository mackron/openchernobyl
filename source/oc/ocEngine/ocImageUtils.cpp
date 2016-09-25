// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocGetMipmapCount(uint32_t baseWidth, uint32_t baseHeight, uint32_t* pCount)
{
    if (pCount == NULL) return OC_RESULT_INVALID_ARGS;
    *pCount = 0;

    if (baseWidth == 0 || baseHeight == 0) return OC_RESULT_INVALID_ARGS;

    *pCount = 1 + (unsigned int)log2f((float)ocMax(baseWidth, baseHeight));
    return OC_RESULT_SUCCESS;
}

ocResult ocGetTotalMipmapDataSize(uint32_t baseWidth, uint32_t baseHeight, uint32_t components, uint32_t alignment, size_t* pSize)
{
    if (pSize == NULL) return OC_RESULT_INVALID_ARGS;
    *pSize = 0;

    if (baseWidth == 0 || baseHeight == 0 || components == 0) return OC_RESULT_INVALID_ARGS;
    if (alignment == 0) alignment = 1;

    size_t totalSize = 0;

    size_t mipmapSizeX = (size_t)baseWidth;
    size_t mipmapSizeY = (size_t)baseHeight;
    for (;;) {
        size_t mipmapDataSize = mipmapSizeX * mipmapSizeY * components;

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
    return OC_RESULT_SUCCESS;
}

ocResult ocGenerateMipmap(uint32_t baseWidth, uint32_t baseHeight, uint32_t components, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmap)
{
    if (baseWidth == 0 || baseHeight == 0 || components == 0 || pBaseData == NULL || pDataOut == NULL) return OC_RESULT_INVALID_ARGS;

    // TODO: Optimized branch for 4 components.

    // The y+1 and x+1 statements are used to handle cases when the base image is not evenly divisible by 2, such as when it's 3x3 going to 1x1. It avoids
    // going out of bounds.
    uint8_t* pRunningDataOut = (uint8_t*)pDataOut;
    for (uint32_t y = 0; y+1 < baseHeight; y += 2) {
        for (uint32_t x = 0; x+1 < baseWidth; x += 2) {
            uint8_t* pBaseTexel = (uint8_t*)pBaseData + (((baseWidth * y) + x) * components);
            for (uint32_t c = 0; c < components; ++c) {
                uint32_t c00 = pBaseTexel[c + 0];
                uint32_t c01 = pBaseTexel[c + components];
                uint32_t c10 = pBaseTexel[c + baseWidth*components];
                uint32_t c11 = pBaseTexel[c + baseWidth*components + components];
                pRunningDataOut[c] = (uint8_t)((c00 + c01 + c10 + c11) >> 2);
            }

            pRunningDataOut += components;
        }

        
    }

    if (pMipmap != NULL) {
        pMipmap->width = baseWidth >> 1;
        pMipmap->height = baseHeight >> 1;
        pMipmap->dataSize = pMipmap->width * pMipmap->height * components;
        pMipmap->offset = 0;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocGenerateMipmaps(uint32_t baseWidth, uint32_t baseHeight, uint32_t components, uint32_t alignment, const void* pBaseData, void* pDataOut, ocMipmapInfo* pMipmaps)
{
    if (baseWidth == 0 || baseHeight == 0 || components == 0 || pBaseData == NULL || pDataOut == NULL) return OC_RESULT_INVALID_ARGS;
    if (alignment == 0) alignment = 1;

    size_t runningOffset = 0;

    size_t prevWidth = baseWidth;
    size_t prevHeight = baseHeight;
    const void* pPrevData = pBaseData;

    size_t iMipmap = 0;
    for (;;) {
        size_t prevDataSize = prevWidth * prevHeight * components;

        void* pDstDataOut = ocOffsetPtr(pDataOut, runningOffset);
        memcpy(pDstDataOut, pPrevData, prevDataSize);

        if (pMipmaps != NULL) {
            pMipmaps[iMipmap].offset = runningOffset;
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

        size_t dstSizeX = ocMax(1, prevWidth >> 1);
        size_t dstSizeY = ocMax(1, prevHeight >> 1);
        pDstDataOut = ocOffsetPtr(pDataOut, runningOffset);
        ocGenerateMipmap(prevWidth, prevHeight, components, pPrevData, pDstDataOut, NULL);

        // The new mipmap becomes the new base.
        prevWidth = dstSizeX;
        prevHeight = dstSizeY;
        pPrevData = pDstDataOut;
        iMipmap += 1;
    }

    return OC_RESULT_SUCCESS;
}