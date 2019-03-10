// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocBuildPrependToFile(const char* filePath, const void* pData, size_t dataSize)
{
    if (filePath == NULL || pData == NULL) {
        return OC_INVALID_ARGS;
    }

    // Just open and read all of the data and then write it back to a new file.
    void* pOriginalData;
    size_t originalDataSize;
    ocResult result = ocOpenAndReadFile(filePath, &pOriginalData, &originalDataSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    FILE* pFileOut = dr_fopen(filePath, "wb");
    if (pFileOut == NULL) {
        ocFree(pOriginalData);
        return OC_ERROR;
    }

    fwrite(pData, 1, dataSize, pFileOut);
    fwrite(pOriginalData, 1, originalDataSize, pFileOut);
    fclose(pFileOut);

    ocFree(pOriginalData);
    return OC_SUCCESS;
}

ocResult ocBuildPrependStringToFile(const char* filePath, const char* text)
{
    return ocBuildPrependToFile(filePath, text, strlen(text));
}


ocResult ocBuildAppendToFile(const char* filePath, const void* pData, size_t dataSize)
{
    if (filePath == NULL || pData == NULL) {
        return OC_INVALID_ARGS;
    }

    // Just open and read all of the data and then write it back to a new file.
    void* pOriginalData;
    size_t originalDataSize;
    ocResult result = ocOpenAndReadFile(filePath, &pOriginalData, &originalDataSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    FILE* pFileOut = dr_fopen(filePath, "wb");
    if (pFileOut == NULL) {
        ocFree(pOriginalData);
        return OC_ERROR;
    }

    fwrite(pOriginalData, 1, originalDataSize, pFileOut);
    fwrite(pData, 1, dataSize, pFileOut);
    fclose(pFileOut);

    ocFree(pOriginalData);
    return OC_SUCCESS;
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



///////////////////////////////////////////////////////////////////////////////
//
// JSON Helpers
//
///////////////////////////////////////////////////////////////////////////////

struct json_value_s* ocJSONLoadFile(const char* pAbsolutePath, struct json_parse_result_s* pParseResult)
{
    ocAssert(pAbsolutePath != NULL);
    ocAssert(pParseResult != NULL);

    char* pFileData;
    size_t fileSize;
    ocResult result = ocOpenAndReadTextFile(pAbsolutePath, &pFileData, &fileSize);
    if (result != OC_SUCCESS) {
        return NULL;
    }

    struct json_value_s* pJSONValue = json_parse_ex((const void*)pFileData, fileSize, json_parse_flags_allow_json5, NULL, NULL, pParseResult);

    ocFree(pFileData);
    return pJSONValue;
}

const char* ocJSONErrorToString(enum json_parse_error_e error)
{
    switch (error)
    {
        case json_parse_error_none: return "json_parse_error_none";
        case json_parse_error_expected_comma_or_closing_bracket: return "json_parse_error_expected_comma_or_closing_bracket";
        case json_parse_error_expected_colon: return "json_parse_error_expected_colon";
        case json_parse_error_expected_opening_quote: return "json_parse_error_expected_opening_quote";
        case json_parse_error_invalid_string_escape_sequence: return "json_parse_error_invalid_string_escape_sequence";
        case json_parse_error_invalid_number_format: return "json_parse_error_invalid_number_format";
        case json_parse_error_invalid_value: return "json_parse_error_invalid_value";
        case json_parse_error_premature_end_of_buffer: return "json_parse_error_premature_end_of_buffer";
        case json_parse_error_invalid_string: return "json_parse_error_invalid_string";
        case json_parse_error_allocator_failed: return "json_parse_error_allocator_failed";
        case json_parse_error_unexpected_trailing_characters: return "json_parse_error_unexpected_trailing_characters";
        case json_parse_error_unknown: return "json_parse_error_unknown";
        default: break;
    }

    return "Unknown error";
}

void ocPrintJSONParseError(const char* pAbsolutePath, struct json_parse_result_s* pParseResult)
{
    ocAssert(pParseResult != NULL);
    printf("%s(%d): error %d: %s\n", pAbsolutePath, (int)pParseResult->error_line_no, (int)pParseResult->error, ocJSONErrorToString((enum json_parse_error_e)pParseResult->error));
}

const char* ocJSONGetStringFromObject(struct json_object_s* pObject, const char* pElementName)
{
    if (pObject == NULL) {
        return NULL;
    }

    for (json_object_element_s* pElement = pObject->start; pElement != NULL; pElement = pElement->next) {
        if (strcmp(pElement->name->string, pElementName) == 0) {
            if (pElement->value->type == json_type_string) {
                return ((struct json_string_s*)pElement->value->payload)->string;
            } else {
                return NULL;    // Found an element of the same name, but it's not a string.
            }
        }
    }

    return NULL;
}

const char* ocJSONGetStringFromValue(struct json_value_s* pValue)
{
    if (pValue == NULL || pValue->type != json_type_string) {
        return NULL;
    }

    return ((struct json_string_s*)pValue->payload)->string;
}

ocResult ocJSONGetBooleanFromValue(struct json_value_s* pValue, ocBool32* pResult)
{
    if (pValue == NULL || pResult == NULL) {
        return OC_INVALID_ARGS;
    }

    if (pValue->type == json_type_true) {
        *pResult = OC_TRUE;
    } else if (pValue->type == json_type_false) {
        *pResult = OC_FALSE;
    } else if (pValue->type == json_type_string) {
        const char* str = ((struct json_string_s*)pValue->payload)->string;
        if (oc_stricmp(str, "false") == 0) {
            *pResult = OC_FALSE;
        } else {
            *pResult = OC_TRUE;
        }
    } else {
        return OC_INVALID_ARGS; // JSON type not recognized.
    }

    return OC_SUCCESS;
}
