// Copyright (C) 2018 David Reid. See included LICENSE file.

// High level helper function for preprending data to a file.
ocResult ocBuildPrependToFile(const char* filePath, const void* pData, size_t dataSize);

// High level helper function for prepending a string to a file.
ocResult ocBuildPrependStringToFile(const char* filePath, const char* text);

// High level helper function for appending data to a file.
ocResult ocBuildAppendToFile(const char* filePath, const void* pData, size_t dataSize);

// High level helper function for appending a string to a file.
ocResult ocBuildAppendStringToFile(const char* filePath, const char* text);



///////////////////////////////////////////////////////////////////////////////
//
// Code Generation
//
///////////////////////////////////////////////////////////////////////////////

// Converts data to a C-style array.
char* ocBuildBufferToCArray(const unsigned char* buffer, size_t size, const char* variableName);

// Converts data to a C-style string.
char* ocBuildBufferToCString(const unsigned char* buffer, size_t size, const char* variableName);



///////////////////////////////////////////////////////////////////////////////
//
// JSON Helpers
//
///////////////////////////////////////////////////////////////////////////////

// Helper for loading and parsing a JSON file.
struct json_value_s* ocJSONLoadFile(const char* pAbsolutePath, struct json_parse_result_s* pParseResult);

// Converts a JSON error code to a string.
const char* ocJSONErrorToString(enum json_parse_error_e error);

// Prints an JSON parse error.
void ocPrintJSONParseError(const char* pAbsolutePath, struct json_parse_result_s* pParseResult);

// Retrieves a string from a named element of an object.
const char* ocJSONGetStringFromObject(struct json_object_s* pObject, const char* pElementName);

// Retrieves the string from a string object.
const char* ocJSONGetStringFromValue(struct json_value_s* pValue);

// Retrieves a boolean value from a value.
//
// Supports standard JSON booleans, integers (0 = false, everything else = 1), strings ("true" and "false").
ocResult ocJSONGetBooleanFromValue(struct json_value_s* pValue, ocBool32* pResult);