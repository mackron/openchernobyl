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


ocResult ocBuildAppendToFile(const char* filePath, const void* pData, size_t dataSize)
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

    fwrite(pOriginalData, 1, originalDataSize, pFileOut);
    fwrite(pData, 1, dataSize, pFileOut);
    fclose(pFileOut);

    ocFree(pOriginalData);
    return OC_RESULT_SUCCESS;
}

ocResult ocBuildAppendStringToFile(const char* filePath, const char* text)
{
    return ocBuildAppendToFile(filePath, text, strlen(text));
}



///////////////////////////////////////////////////////////////////////////////
//
// Code Generation
//
///////////////////////////////////////////////////////////////////////////////

char* ocBuildBufferToCArray(const unsigned char* buffer, size_t size, const char* variableName)
{
    const unsigned int bytesPerLine = 16;
    const char* header = "static const unsigned char ";
    const char* declarationTail = "[] = {\n";

    size_t headerLen          = strlen(header);
    size_t variableNameLen    = strlen(variableName);
    size_t declarationTailLen = strlen(declarationTail);

    size_t totalLen = headerLen + variableNameLen + declarationTailLen;
    totalLen += size * 6;                                                // x6 because we store 6 character's per byte.
    totalLen += (size / bytesPerLine + 1) * 4;                           // Indentation.
    totalLen += 2;                                                       // +2 for the "};" at the end.

    char* output = (char*)ocMalloc(totalLen);                            // No need for +1 for the null terminator because the last byte will not have a trailing "," which leaves room.

    char* runningOutput = output;
    ocCopyMemory(runningOutput, header, headerLen);
    runningOutput += headerLen;

    ocCopyMemory(runningOutput, variableName, variableNameLen);
    runningOutput += variableNameLen;

    ocCopyMemory(runningOutput, declarationTail, declarationTailLen);
    runningOutput += declarationTailLen;

    for (unsigned int i = 0; i < size; ++i)
    {
        const unsigned char byte = buffer[i];

        if ((i % bytesPerLine) == 0) {
            runningOutput[0] = ' ';
            runningOutput[1] = ' ';
            runningOutput[2] = ' ';
            runningOutput[3] = ' ';
            runningOutput += 4;
        }

        runningOutput[0] = '0';
        runningOutput[1] = 'x';
        runningOutput[2] = ((byte >>  4) + '0'); if (runningOutput[2] > '9') runningOutput[2] += 7;
        runningOutput[3] = ((byte & 0xF) + '0'); if (runningOutput[3] > '9') runningOutput[3] += 7;
        runningOutput += 4;

        if (i + 1 < size) {
            *runningOutput++ = ',';
        }
        
        if ((i % bytesPerLine)+1 == bytesPerLine || i + 1 == size) {
            *runningOutput++ = '\n';
        } else {
            *runningOutput++ = ' ';
        }
    }

    runningOutput[0] = '}';
    runningOutput[1] = ';';
    runningOutput[2] = '\0';
    return output;
}

char* ocBuildBufferToCString(const unsigned char* buffer, size_t size, const char* variableName)
{
    if (buffer == NULL) return NULL;

    const char* input = (const char*)buffer;
    const char* indent = "";

    ocString output = ocMakeString("");

    // Don't include the variable declaration if no variable name was specified.
    if (variableName != NULL) {
        indent = "    ";
        output = ocMakeString("static const char* ");
        output = ocAppendString(output, variableName);
        output = ocAppendString(output, " = {\n");
    }

    output = ocAppendString(ocAppendString(output, indent), "\"");   // <-- Begin the first line with a double-quote.
    {
        // At the momement all we're doing is wrapping each line with " ... \n", but later on we'll want to do
        // proper tab formatting and UTF-8 conversion.
        for (unsigned int ichar = 0; ichar < size; ++ichar) {
            switch (input[ichar]) {
                case '\n': output = ocAppendString(output, "\\n"); output = ocAppendString(output, "\"\n"); output = ocAppendString(ocAppendString(output, indent), "\""); break;  // <-- Terminate the line with a double-quote and place the double-quote for the following line.
                case '\r': output = ocAppendString(output, "\\r"); break;
                case '\t': output = ocAppendString(output, "\\t"); break;
                case '\"': output = ocAppendString(output, "\\\""); break;
                case '\\': output = ocAppendString(output, "\\\\"); break;
                default:
                {
                    // TODO: Check for non-ASCII characters and add support for UTF-8 hex characters.
                    output = ocAppendStringLength(output, &input[ichar], 1);  
                } break;
            }
        }
    }
    output = ocAppendString(output, "\"");   // <-- End the last line with a double-quote.
    if (variableName != NULL) {
        output = ocAppendString(output, "\n};");
    }

    return output;
}