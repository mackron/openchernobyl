// Copyright (C) 2018 David Reid. See included LICENSE file.

OC_PRIVATE ocResult ocStreamReader_OnRead_File(void* pUserData, void* pDataOut, ocSizeT bytesToRead, ocSizeT* pBytesRead)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    return ocFileRead(pReader->pFile, pDataOut, bytesToRead, pBytesRead);
}

OC_PRIVATE ocResult ocStreamReader_OnSeek_File(void* pUserData, ocInt64 bytesToSeek, ocSeekOrigin origin)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    return ocFileSeek(pReader->pFile, bytesToSeek, origin);
}

OC_PRIVATE ocResult ocStreamReader_OnTell_File(void* pUserData, ocUInt64* pPos)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    return ocFileTell(pReader->pFile, pPos);
}

OC_PRIVATE ocBool32 ocStreamReader_OnAtEnd_File(void* pUserData)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    return ocAtEOF(pReader->pFile);
}

ocResult ocStreamReaderInit(ocFile* pFile, ocStreamReader* pReader)
{
    if (pFile == NULL) return OC_INVALID_ARGS;

    ocResult result = ocStreamReaderInit(ocStreamReader_OnRead_File, ocStreamReader_OnSeek_File, ocStreamReader_OnTell_File, ocStreamReader_OnAtEnd_File, (void*)pReader, pReader);
    if (result != OC_SUCCESS) {
        return result;
    }

    pReader->pFile = pFile;

    return OC_SUCCESS;
}


OC_PRIVATE ocResult ocStreamReader_OnRead_Memory(void* pUserData, void* pDataOut, ocSizeT bytesToRead, ocSizeT* pBytesRead)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    if (pBytesRead) {
        *pBytesRead = 0;
    }

    // Don't do anything if we're at the end of the buffer.
    if (pReader->memory.currentPos == pReader->memory.dataSize) {
        return OC_AT_END_OF_FILE;
    }

    // Only read as much as we can.
    ocSizeT bytesRemaining = pReader->memory.dataSize - pReader->memory.currentPos;
    if (bytesToRead > bytesRemaining) {
        bytesToRead = bytesRemaining;
    }

    ocCopyMemory(pDataOut, pReader->memory.pData + pReader->memory.currentPos, bytesToRead);
    pReader->memory.currentPos += bytesToRead;


    if (pBytesRead) {
        *pBytesRead = bytesToRead;
    }

    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocStreamReader_OnSeek_Memory(void* pUserData, ocInt64 bytesToSeek, ocSeekOrigin origin)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    switch (origin)
    {
        case ocSeekOrigin_Current:
        {
            if (bytesToSeek > 0) {
                if (pReader->memory.currentPos + bytesToSeek > pReader->memory.dataSize) {
                    return OC_INVALID_ARGS;  // Seeking too far forward.
                }

                pReader->memory.currentPos += (ocSizeT)bytesToSeek;
            } else {
                if (pReader->memory.currentPos + bytesToSeek < 0) {
                    return OC_INVALID_ARGS;  // Seeking too far backwards.
                }

                pReader->memory.currentPos -= (ocSizeT)(-bytesToSeek);
            }
        } break;

        case ocSeekOrigin_Start:
        {
            if (bytesToSeek < 0) {
                return OC_INVALID_ARGS;  // Does not make sense to use a negative seek when seeking from the start.
            }
            if (bytesToSeek > (ocInt64)pReader->memory.dataSize) {
                return OC_INVALID_ARGS;  // Seeking too far.
            }

            pReader->memory.currentPos = (ocSizeT)bytesToSeek;
        } break;

        case ocSeekOrigin_End:
        {
            // Always make the seek positive for simplicity.
            if (bytesToSeek < 0) {
                bytesToSeek = -bytesToSeek;
            }

            if (bytesToSeek > (ocInt64)pReader->memory.dataSize) {
                return OC_INVALID_ARGS;  // Seeking too far.
            }

            pReader->memory.currentPos = pReader->memory.dataSize - (ocSizeT)bytesToSeek;
        } break;
    }

    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocStreamReader_OnTell_Memory(void* pUserData, ocUInt64* pPos)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    *pPos = pReader->memory.currentPos;
    return OC_SUCCESS;
}

OC_PRIVATE ocBool32 ocStreamReader_OnAtEnd_Memory(void* pUserData)
{
    ocStreamReader* pReader = (ocStreamReader*)pUserData;
    ocAssert(pReader != NULL);

    return pReader->memory.currentPos == pReader->memory.dataSize;
}

ocResult ocStreamReaderInit(const void* pData, ocSizeT dataSize, ocStreamReader* pReader)
{
    if (pData == NULL) return OC_INVALID_ARGS;

    ocResult result = ocStreamReaderInit(ocStreamReader_OnRead_Memory, ocStreamReader_OnSeek_Memory, ocStreamReader_OnTell_Memory, ocStreamReader_OnAtEnd_Memory, (void*)pReader, pReader);
    if (result != OC_SUCCESS) {
        return result;
    }

    pReader->memory.pData = (const ocUInt8*)pData;
    pReader->memory.dataSize = dataSize;
    pReader->memory.currentPos = 0;

    return OC_SUCCESS;
}

ocResult ocStreamReaderInit(ocStreamReader_OnReadProc onRead, ocStreamReader_OnSeekProc onSeek, ocStreamReader_OnTellProc onTell, ocStreamReader_OnAtEndProc onAtEnd, void* pUserData, ocStreamReader* pReader)
{
    if (pReader == NULL) return OC_INVALID_ARGS;

    ocZeroObject(pReader);
    pReader->onRead    = onRead;
    pReader->onSeek    = onSeek;
    pReader->onTell    = onTell;
    pReader->onAtEnd   = onAtEnd;
    pReader->pUserData = pUserData;

    return OC_SUCCESS;
}


ocResult ocStreamReaderUninit(ocStreamReader* pReader)
{
    if (pReader == NULL) return OC_INVALID_ARGS;
    return OC_SUCCESS;
}


ocResult ocStreamReaderRead(ocStreamReader* pReader, void* pDataOut, ocSizeT bytesToRead, ocSizeT* pBytesRead)
{
    if (pReader == NULL) return OC_INVALID_ARGS;

    if (pReader->onRead == NULL) {
        return OC_FEATURE_NOT_SUPPORTED;
    }

    return pReader->onRead(pReader->pUserData, pDataOut, bytesToRead, pBytesRead);
}

ocResult ocStreamReaderSeek(ocStreamReader* pReader, ocInt64 bytesToSeek, ocSeekOrigin origin)
{
    if (pReader == NULL) return OC_INVALID_ARGS;

    if (pReader->onSeek == NULL) {
        return OC_FEATURE_NOT_SUPPORTED;
    }

    return pReader->onSeek(pReader->pUserData, bytesToSeek, origin);
}

ocResult ocStreamReaderTell(ocStreamReader* pReader, ocUInt64* pPos)
{
    if (pReader == NULL) return OC_INVALID_ARGS;

    if (pReader->onTell == NULL) {
        return OC_FEATURE_NOT_SUPPORTED;
    }

    return pReader->onTell(pReader->pUserData, pPos);
}

ocResult ocStreamReaderSize(ocStreamReader* pReader, ocUInt64* pSize)
{
    if (pSize) *pSize = 0;  // Safety.

    ocUInt64 currentPos;
    ocResult result = ocStreamReaderTell(pReader, &currentPos);
    if (result != OC_SUCCESS) {
        return result;
    }

    result = ocStreamReaderSeek(pReader, 0, ocSeekOrigin_End);
    if (result != OC_SUCCESS) {
        return result;
    }

    ocUInt64 streamSize;
    result = ocStreamReaderTell(pReader, &streamSize);
    if (result != OC_SUCCESS) {
        ocStreamReaderSeek(pReader, (ocInt64)currentPos, ocSeekOrigin_Start);   // Try putting the reader back to where it was.
        return result;
    }

    result = ocStreamReaderSeek(pReader, (ocInt64)currentPos, ocSeekOrigin_Start);
    if (result != OC_SUCCESS) {
        return result;
    }

    if (pSize) *pSize = streamSize;
    return OC_SUCCESS;
}

ocBool32 ocStreamReaderAtEnd(ocStreamReader* pReader)
{
    if (pReader == NULL) return OC_TRUE;

    if (pReader->onAtEnd == NULL) {
        return OC_TRUE;
    }

    return pReader->onAtEnd(pReader->pUserData);
}