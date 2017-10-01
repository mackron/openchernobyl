// Copyright (C) 2017 David Reid. See included LICENSE file.

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