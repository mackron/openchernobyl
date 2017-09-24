// Copyright (C) 2017 David Reid. See included LICENSE file.

OC_PRIVATE ocResult ocStreamWriter_OnWrite_File(void* pUserData, const void* pData, ocSizeT bytesToWrite, ocSizeT* pBytesWritten)
{
    ocStreamWriter* pWriter = (ocStreamWriter*)pUserData;
    ocAssert(pWriter != NULL);

    return ocFileWrite(pWriter->pFile, pData, bytesToWrite, pBytesWritten);
}

ocResult ocStreamWriterInit(ocFile* pFile, ocStreamWriter* pWriter)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;

    ocResult result = ocStreamWriterInit(ocStreamWriter_OnWrite_File, (void*)pWriter, pWriter);
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
    if (pWriter->memory.dataSize + bytesToWrite > pWriter->memory.bufferSize) {
        // Resize.
        ocSizeT newBufferSize = (pWriter->memory.bufferSize == 0) ? bytesToWrite : pWriter->memory.bufferSize*2;
        ocUInt8* pNewBuffer = (ocUInt8*)ocRealloc(pWriter->memory.pBuffer, newBufferSize);
        if (pNewBuffer == NULL) {
            return OC_RESULT_OUT_OF_MEMORY;
        }

        pWriter->memory.pBuffer = pNewBuffer;
        pWriter->memory.bufferSize = newBufferSize;
    }

    ocCopyMemory(pWriter->memory.pBuffer + pWriter->memory.dataSize, pData, bytesToWrite);
    pWriter->memory.dataSize += bytesToWrite;
    *pBytesWritten = bytesToWrite;

    // Update the output variables after every write.
    *pWriter->memory.ppData = (void*)pWriter->memory.pBuffer;
    *pWriter->memory.pDataSize = pWriter->memory.dataSize;

    return OC_RESULT_SUCCESS;
}

ocResult ocStreamWriterInit(void** ppData, size_t* pDataSize, ocStreamWriter* pWriter)
{
    if (ppData == NULL || pDataSize == NULL) return OC_RESULT_INVALID_ARGS;

    ocResult result = ocStreamWriterInit(ocStreamWriter_OnWrite_Memory, (void*)pWriter, pWriter);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pWriter->memory.ppData = ppData;
    pWriter->memory.pDataSize = pDataSize;

    return OC_RESULT_SUCCESS;
}

ocResult ocStreamWriterInit(ocStreamWriter_OnWriteProc onWrite, void* pUserData, ocStreamWriter* pWriter)
{
    if (pWriter == NULL || onWrite == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pWriter);
    pWriter->onWrite = onWrite;
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
