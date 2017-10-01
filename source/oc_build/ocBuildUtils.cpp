// Copyright (C) 2017 David Reid. See included LICENSE file.

ocResult ocBuildPrependToFile(const char* filePath, const void* pData, size_t dataSize)
{
    if (filePath == NULL || pData == NULL) return OC_RESULT_INVALID_ARGS;

    // Just open and read all of the data and then write it back to a new file.
    void* pOriginalData;
    size_t originalDataSize;
    ocResult result = ocOpenAndReadFile(filePath, &pOriginalData, &originalDataSize);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    FILE* pFileOut = dr_fopen(filePath, "wb");
    if (pFileOut == NULL) {
        ocFree(pOriginalData);
        return OC_RESULT_UNKNOWN_ERROR;
    }

    fwrite(pData, 1, dataSize, pFileOut);
    fwrite(pOriginalData, 1, originalDataSize, pFileOut);
    fclose(pFileOut);

    ocFree(pOriginalData);
    return OC_RESULT_SUCCESS;
}

ocResult ocBuildPrependStringToFile(const char* filePath, const char* text)
{
    return ocBuildPrependToFile(filePath, text, strlen(text));
}
