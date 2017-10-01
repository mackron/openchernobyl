// Copyright (C) 2017 David Reid. See included LICENSE file.

// High level helper function for preprending data to a file.
ocResult ocBuildPrependToFile(const char* filePath, const void* pData, size_t dataSize);

// High level helper function for prepending a string to a file.
ocResult ocBuildPrependStringToFile(const char* filePath, const char* text);