// Copyright (C) 2018 David Reid. See included LICENSE file.

OC_PRIVATE ocResult ocStreamWriter_OnWrite_File(void* pUserData, const void* pData, ocSizeT bytesToWrite, ocSizeT* pBytesWritten)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    return ocFileWrite(pWriter->pFile, pData, bytesToWrite, pBytesWritten);
}

OC_PRIVATE ocResult ocStreamWriter_OnSeek_File(void* pUserData, ocInt64 bytesToSeek, ocSeekOrigin origin)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    return ocFileSeek(pWriter->pFile, bytesToSeek, origin);
}

OC_PRIVATE ocResult ocStreamWriter_OnTell_File(void* pUserData, ocUInt64* pPos)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    return ocFileTell(pWriter->pFile, pPos);
}

OC_PRIVATE ocResult ocStreamWriter_OnSize_File(void* pUserData, ocUInt64* pSize)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    return ocFileSize(pWriter->pFile, pSize);
}

ocResult ocStreamWriterInit(ocFile* pFile, ocStreamWriter* pWriter)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;

    ocResult result = ocStreamWriterInit(ocStreamWriter_OnWrite_File, ocStreamWriter_OnSeek_File, ocStreamWriter_OnTell_File, ocStreamWriter_OnSize_File, (void*)pWriter, pWriter);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pWriter->pFile = pFile;

    return OC_RESULT_SUCCESS;
}


OC_PRIVATE ocResult ocStreamWriter_OnWrite_Memory(void* pUserData, const void* pData, ocSizeT bytesToWrite, ocSizeT* pBytesWritten)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    // If there's not enough room, make room. We just use a simple doubling of the buffer size for each resize.
    ocSizeT minRequiredBufferSize = pWriter->memory.currentPos + bytesToWrite;
    if (minRequiredBufferSize > pWriter->memory.bufferSize) {
        // Resize.
        ocSizeT newBufferSize = (pWriter->memory.bufferSize == 0) ? bytesToWrite : pWriter->memory.bufferSize*2;
        if (newBufferSize < minRequiredBufferSize) {
            newBufferSize = minRequiredBufferSize;
        }

        ocUInt8* pNewBuffer = (ocUInt8*)ocRealloc(pWriter->memory.pBuffer, newBufferSize);
        if (pNewBuffer == NULL) {
            return OC_RESULT_OUT_OF_MEMORY;
        }

        pWriter->memory.pBuffer = pNewBuffer;
        pWriter->memory.bufferSize = newBufferSize;
    }

    ocCopyMemory(pWriter->memory.pBuffer + pWriter->memory.currentPos, pData, bytesToWrite);
    pWriter->memory.currentPos += bytesToWrite;

    if (pWriter->memory.dataSize  < pWriter->memory.currentPos) {
        pWriter->memory.dataSize += pWriter->memory.currentPos;
    }
    
    if (pBytesWritten) *pBytesWritten = bytesToWrite;

    // Update the output variables after every write.
    *pWriter->memory.ppData = (void*)pWriter->memory.pBuffer;
    *pWriter->memory.pDataSize = pWriter->memory.dataSize;

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocStreamWriter_OnSeek_Memory(void* pUserData, ocInt64 bytesToSeek, ocSeekOrigin origin)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    switch (origin)
    {
        case ocSeekOrigin_Current:
        {
            if (bytesToSeek > 0) {
                if (pWriter->memory.currentPos + bytesToSeek > pWriter->memory.dataSize) {
                    return OC_RESULT_INVALID_ARGS;  // Seeking too far forward.
                }

                pWriter->memory.currentPos += (ocSizeT)bytesToSeek;
            } else {
                if (pWriter->memory.currentPos + bytesToSeek < 0) {
                    return OC_RESULT_INVALID_ARGS;  // Seeking too far backwards.
                }

                pWriter->memory.currentPos -= (ocSizeT)(-bytesToSeek);
            }
        } break;

        case ocSeekOrigin_Start:
        {
            if (bytesToSeek < 0) {
                return OC_RESULT_INVALID_ARGS;  // Does not make sense to use a negative seek when seeking from the start.
            }
            if (bytesToSeek > (ocInt64)pWriter->memory.dataSize) {
                return OC_RESULT_INVALID_ARGS;  // Seeking too far.
            }

            pWriter->memory.currentPos = (ocSizeT)bytesToSeek;
        } break;

        case ocSeekOrigin_End:
        {
            // Always make the seek positive for simplicity.
            if (bytesToSeek < 0) {
                bytesToSeek = -bytesToSeek;
            }

            if (bytesToSeek > (ocInt64)pWriter->memory.dataSize) {
                return OC_RESULT_INVALID_ARGS;  // Seeking too far.
            }

            pWriter->memory.currentPos = pWriter->memory.dataSize - (ocSizeT)bytesToSeek;
        } break;
    }

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocStreamWriter_OnTell_Memory(void* pUserData, ocUInt64* pPos)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    *pPos = pWriter->memory.currentPos;
    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocStreamWriter_OnSize_Memory(void* pUserData, ocUInt64* pSize)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    *pSize = pWriter->memory.dataSize;
    return OC_RESULT_SUCCESS;
}

ocResult ocStreamWriterInit(void** ppData, size_t* pDataSize, ocStreamWriter* pWriter)
{
    if (ppData == NULL || pDataSize == NULL) return OC_RESULT_INVALID_ARGS;

    ocResult result = ocStreamWriterInit(ocStreamWriter_OnWrite_Memory, ocStreamWriter_OnSeek_Memory, ocStreamWriter_OnTell_Memory, ocStreamWriter_OnSize_Memory, (void*)pWriter, pWriter);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pWriter->memory.ppData = ppData;
    pWriter->memory.pDataSize = pDataSize;

    *ppData = NULL;
    *pDataSize = 0;

    return OC_RESULT_SUCCESS;
}


ocResult ocStreamWriterInit(ocStreamWriter_OnWriteProc onWrite, ocStreamWriter_OnSeekProc onSeek, ocStreamWriter_OnTellProc onTell, ocStreamWriter_OnSizeProc onSize, void* pUserData, ocStreamWriter* pWriter)
{
    if (pWriter == NULL || onWrite == NULL || onSeek == NULL || onTell == NULL || onSize == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pWriter);
    pWriter->onWrite = onWrite;
    pWriter->onSeek = onSeek;
    pWriter->onTell = onTell;
    pWriter->onSize = onSize;
    pWriter->pUserData = pUserData;

    return OC_RESULT_SUCCESS;
}

ocResult ocStreamWriterUninit(ocStreamWriter* pWriter)
{
    if (pWriter == NULL) return OC_RESULT_INVALID_ARGS;
    return OC_RESULT_SUCCESS;
}

ocResult ocStreamWriterWrite(ocStreamWriter* pWriter, const void* pData, ocSizeT bytesToWrite, ocSizeT* pBytesWritten)
{
    if (pWriter == NULL || pData == NULL) return OC_RESULT_INVALID_ARGS;

    ocAssert(pWriter->onWrite != NULL);
    return pWriter->onWrite(pWriter->pUserData, pData, bytesToWrite, pBytesWritten);
}

ocResult ocStreamWriterWriteString(ocStreamWriter* pWriter, const char* str)
{
    if (pWriter == NULL || str == NULL) return OC_RESULT_INVALID_ARGS;
    return ocStreamWriterWrite(pWriter, str, strlen(str)+1, NULL);  // <-- Include null terminator in output.
}

ocResult ocStreamWriterSeek(ocStreamWriter* pWriter, ocInt64 bytesToSeek, ocSeekOrigin origin)
{
    if (pWriter == NULL || pWriter->onSeek == NULL) return OC_RESULT_INVALID_ARGS;
    return pWriter->onSeek(pWriter->pUserData, bytesToSeek, origin);
}

ocResult ocStreamWriterTell(ocStreamWriter* pWriter, ocUInt64* pPos)
{
    if (pWriter == NULL || pWriter->onTell == NULL || pPos == NULL) return OC_RESULT_INVALID_ARGS;
    return pWriter->onTell(pWriter->pUserData, pPos);
}

ocResult ocStreamWriterSize(ocStreamWriter* pWriter, ocUInt64* pSize)
{
    if (pWriter == NULL || pWriter->onSize == NULL || pSize == NULL) return OC_RESULT_INVALID_ARGS;
    return pWriter->onSize(pWriter->pUserData, pSize);
}
