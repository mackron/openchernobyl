// Copyright (C) 2017 David Reid. See included LICENSE file.

OC_PRIVATE ocResult ocToResult(drfs_result result)
{
    switch (result)
    {
        case drfs_success:                  return OC_RESULT_SUCCESS;
        case drfs_unknown_error:            return OC_RESULT_UNKNOWN_ERROR;
        case drfs_invalid_args:             return OC_RESULT_INVALID_ARGS;
        case drfs_does_not_exist:           return OC_RESULT_DOES_NOT_EXIST;
        case drfs_already_exists:           return OC_RESULT_ALREADY_EXISTS;
        case drfs_permission_denied:        return OC_RESULT_PERMISSION_DENIED;
        case drfs_too_many_open_files:      return OC_RESULT_TOO_MANY_OPEN_FILES;
        case drfs_no_backend:               return OC_RESULT_NO_BACKEND;
        case drfs_out_of_memory:            return OC_RESULT_OUT_OF_MEMORY;
        case drfs_not_in_write_directory:   return OC_RESULT_FILE_NOT_IN_WRITE_DIRECTORY;
        case drfs_path_too_long:            return OC_RESULT_PATH_TOO_LONG;
        case drfs_no_space:                 return OC_RESULT_NO_SPACE;
        case drfs_not_directory:            return OC_RESULT_NOT_DIRECTORY;
        case drfs_too_large:                return OC_RESULT_TOO_LARGE;
        case drfs_at_end_of_file:           return OC_RESULT_AT_END_OF_FILE;
        case drfs_invalid_archive:          return OC_RESULT_INVALID_ARCHIVE;
        case drfs_negative_seek:            return OC_RESULT_NEGATIVE_SEEK;
    }

    return OC_RESULT_UNKNOWN_ERROR;
}

OC_PRIVATE drfs_seek_origin ocToDRFSSeekOrigin(ocSeekOrigin origin)
{
    switch (origin)
    {
        case ocSeekOrigin_Start:    return drfs_origin_start;
        case ocSeekOrigin_End:      return drfs_origin_end;
        case ocSeekOrigin_Current:  return drfs_origin_current;
        default:                    return drfs_origin_current;
    }
}


ocResult ocFileSystemInit(ocEngineContext* pEngine, ocFileSystem* pFS)
{
    if (pFS == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pFS);

    if (pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    pFS->pEngine = pEngine;

    if (drfs_init(&pFS->internalFS) != drfs_success) {
        return OC_RESULT_UNKNOWN_ERROR;
    }

    // The base directories should always be relative to the executable.
    char exePath[OC_MAX_PATH];
    if (dr_get_executable_directory_path(exePath, sizeof(exePath))) {
        char dataPath[OC_MAX_PATH];
        drpath_copy_and_append(dataPath, sizeof(dataPath), exePath, "data");
        drfs_add_base_directory(&pFS->internalFS, dataPath);

        // The executable's directory should be the lowest priority base directory.
        drfs_add_base_directory(&pFS->internalFS, exePath);
    }

    return OC_RESULT_SUCCESS;
}

void ocFileSystemUninit(ocFileSystem* pFS)
{
    if (pFS == NULL) return;
    drfs_uninit(&pFS->internalFS);
}


ocResult ocGetFileInfo(ocFileSystem* pFS, const char* relativePath, ocFileInfo* pInfo)
{
    if (pInfo != NULL) ocZeroObject(pInfo);
    return ocToResult(drfs_get_file_info(&pFS->internalFS, relativePath, pInfo));
}

ocResult ocFindAbsoluteFilePath(ocFileSystem* pFS, const char* relativePath, char* absolutePath, size_t absolutePathSize)
{
    if (absolutePath == NULL) return OC_RESULT_INVALID_ARGS;
    if (absolutePathSize > 0) absolutePath[0] = '\0';

    if (pFS == NULL || relativePath == NULL) return OC_RESULT_INVALID_ARGS;

    return ocToResult(drfs_find_absolute_path(&pFS->internalFS, relativePath, absolutePath, absolutePathSize));
}



ocResult ocFileOpen(ocFileSystem* pFS, const char* path, unsigned int accessMode, ocFile* pFile)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pFile);

    if (pFS == NULL || path == NULL) return OC_RESULT_INVALID_ARGS;

    drfs_result result = drfs_open(&pFS->internalFS, path, accessMode, &pFile->pInternalFile);
    if (result != drfs_success) {
        return ocToResult(result);
    }

    return OC_RESULT_SUCCESS;
}

void ocFileClose(ocFile* pFile)
{
    if (pFile == NULL) return;
    drfs_close(pFile->pInternalFile);
}

ocResult ocFileRead(ocFile* pFile, void* pDataOut, size_t bytesToRead, size_t* pBytesReadOut)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;
    return ocToResult(drfs_read(pFile->pInternalFile, pDataOut, bytesToRead, pBytesReadOut));
}

ocResult ocFileWrite(ocFile* pFile, const void* pData, size_t bytesToWrite, size_t* pBytesWrittenOut)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;
    return ocToResult(drfs_write(pFile->pInternalFile, pData, bytesToWrite, pBytesWrittenOut));
}

ocResult ocFileSeek(ocFile* pFile, int64_t bytesToSeek, ocSeekOrigin origin)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;
    return ocToResult(drfs_seek(pFile->pInternalFile, bytesToSeek, ocToDRFSSeekOrigin(origin)));
}

uint64_t ocFileTell(ocFile* pFile)
{
    if (pFile == NULL) return 0;
    return drfs_tell(pFile->pInternalFile);
}

uint64_t ocFileSize(ocFile* pFile)
{
    if (pFile == NULL) return 0;
    return drfs_size(pFile->pInternalFile);
}

void ocFileFlush(ocFile* pFile)
{
    if (pFile == NULL) return;
    drfs_flush(pFile->pInternalFile);
}

ocBool32 ocAtEOF(ocFile* pFile)
{
    if (pFile == NULL) return false;
    return drfs_eof(pFile->pInternalFile);
}



///////////////////////////////////////////////////////////////////////////////
//
// High Level File API
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocFileWriteString(ocFile* pFile, const char* str)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;
    return ocToResult(drfs_write_string(pFile->pInternalFile, str)); 
}

ocResult ocFileWriteLine(ocFile* pFile, const char* str)
{
    if (pFile == NULL) return OC_RESULT_INVALID_ARGS;
    return ocToResult(drfs_write_line(pFile->pInternalFile, str)); 
}



///////////////////////////////////////////////////////////////////////////////
//
// Known Folders and Files
//
///////////////////////////////////////////////////////////////////////////////

void ocGetLogFolderPath(ocFileSystem* pFS, char* pathOut, size_t pathOutSize)
{
    if (pathOut == NULL || pathOutSize == 0) return;

    char logFolderPath[DRFS_MAX_PATH];
    if (!ocIsPortable(pFS->pEngine) && dr_get_log_folder_path(logFolderPath, sizeof(logFolderPath))) {
        // If the log folder path is relative, assume it's relative to the executable.
        if (drpath_is_relative(logFolderPath)) {
            dr_get_executable_path(pathOut, pathOutSize);
            drpath_base_path(pathOut);
            drpath_append(pathOut, pathOutSize, logFolderPath);
        } else {
            strcpy_s(pathOut, pathOutSize, logFolderPath);
        }

        // The folder path needs to be namespaced based on the game name.
        drpath_append(pathOut, pathOutSize, OC_CONFIG_NAME);
    } else {
        // We're either running the portable version of we've failed to retrieve the per-user log folder path so we'll just fall back to
        // the executable's directory.
        dr_get_executable_path(pathOut, pathOutSize);
        drpath_base_path(pathOut);
        drpath_append(pathOut, pathOutSize, "var/log");
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// General File System APIs
//
///////////////////////////////////////////////////////////////////////////////

#ifdef OC_WIN32
ocBool32 ocIsDirectory_Win32(const char* path)
{
    ocAssert(path != NULL);

    DWORD attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

ocString ocGetCurrentDirectory_Win32()
{
    DWORD len = GetCurrentDirectoryA(0, NULL);
    if (len == 0) {
        return NULL;
    }

    char* pDir = (char*)ocMalloc(len);
    if (pDir == NULL) {
        return NULL;    // Out of memory.
    }

    len = GetCurrentDirectoryA(len, pDir);
    if (len == 0) {
        ocFree(pDir);
        return NULL;
    }

    ocString result = ocMakeString(pDir);
    ocFree(pDir);

    return result;
}

ocResult ocSetCurrentDirectory_Win32(const char* path)
{
    if (SetCurrentDirectoryA(path) == 0) {
        return OC_RESULT_UNKNOWN_ERROR;
    }

    return OC_RESULT_SUCCESS;
}
#endif

#ifdef OC_POSIX
ocBool32 ocIsDirectory_Posix(const char* path)
{
    ocAssert(path != NULL);

    struct stat info;
    if (stat(path, &info) != 0) {
        return OC_FALSE;   // Likely the folder doesn't exist.
    }

    return (info.st_mode & S_IFDIR) != 0;
}

ocString ocGetCurrentDirectory_Posix()
{
    char* pDirTemp = getcwd(NULL, 0);
    if (pDir == NULL) {
        return NULL;
    }

    ocString result = ocMakeString(pDirTemp);
    free(pDirTemp);

    return result;
}

ocResult ocSetCurrentDirectory_Posix(const char* path)
{
    if (chdir(path) != 0) {
        return OC_RESULT_UNKNOWN_ERROR;
    }

    return OC_RESULT_SUCCESS;
}
#endif

ocBool32 ocIsDirectory(const char* path)
{
#ifdef OC_WIN32
    return ocIsDirectory_Win32(path);
#endif
#ifdef OC_POSIX
    return ocIsDirectory_Posix(path);
#endif
}

ocString ocGetCurrentDirectory()
{
#ifdef OC_WIN32
    return ocGetCurrentDirectory_Win32();
#endif
#ifdef OC_POSIX
    return ocGetCurrentDirectory_Posix();
#endif
}

ocResult ocSetCurrentDirectory(const char* path)
{
#ifdef OC_WIN32
    return ocSetCurrentDirectory_Win32(path);
#endif
#ifdef OC_POSIX
    return ocSetCurrentDirectory_Posix(path);
#endif
}

ocResult ocCreateDirectory(const char* directoryPath)
{
    if (directoryPath == NULL) return OC_RESULT_INVALID_ARGS;

#ifdef OC_WIN32
    if (CreateDirectoryA(directoryPath, NULL) == 0) {
        return OC_RESULT_UNKNOWN_ERROR;
    }
    return OC_RESULT_SUCCESS;
#endif
#ifdef OC_POSIX
    if (mkdir(directoryPath, 0777) != 0) {
        return OC_REUSLT_UNKNOWN_ERROR;
    }
    return OC_RESULT_SUCCESS;
#endif
}

ocResult ocCreateDirectoryRecursive(const char* directoryPath)
{
    if (directoryPath == NULL || directoryPath[0] == '\0') {
        return OC_RESULT_INVALID_ARGS;
    }

    // TODO: Don't restrict this to OC_MAX_PATH.
    // All we need to do is iterate over every segment in the path and try creating the directory.
    char runningPath[OC_MAX_PATH];
    ocZeroMemory(runningPath, sizeof(runningPath));

    size_t i = 0;
    for (;;) {
        if (i >= sizeof(runningPath)-1) {
            return OC_RESULT_PATH_TOO_LONG;   // Path is too long.
        }

        if (directoryPath[0] == '\0' || directoryPath[0] == '/' || directoryPath[0] == '\\') {
            if (runningPath[0] != '\0' && !(runningPath[1] == ':' && runningPath[2] == '\0')) {   // <-- If the running path is empty, it means we're trying to create the root directory.
                if (!ocIsDirectory(runningPath)) {
                    ocResult result = ocCreateDirectory(runningPath);
                    if (result != OC_RESULT_SUCCESS) {
                        return result;
                    }
                }
            }

            //printf("%s\n", runningPath);
            runningPath[i++] = '/';
            runningPath[i]   = '\0';

            if (directoryPath[0] == '\0') {
                break;
            }
        } else {
            runningPath[i++] = directoryPath[0];
        }

        directoryPath += 1;
    }

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocResultFromERRNO(errno_t e)    // TODO: Move this API somewhere more generic.
{
    switch (e) {
        case EACCES: return OC_RESULT_PERMISSION_DENIED;
        case EEXIST: return OC_RESULT_ALREADY_EXISTS;
        case EINVAL: return OC_RESULT_INVALID_ARGS;
        case EMFILE: return OC_RESULT_TOO_MANY_OPEN_FILES;
        case ENOENT: return OC_RESULT_DOES_NOT_EXIST;
        case ENOMEM: return OC_RESULT_OUT_OF_MEMORY;
        default: break;
    }

    return OC_RESULT_UNKNOWN_ERROR;
}

OC_PRIVATE ocResult ocFOpen(const char* filePath, const char* openMode, FILE** ppFileOut)
{
    FILE* pFile;
#ifdef _MSC_VER
    if (fopen_s(&pFile, filePath, openMode) != 0) {
        goto on_error;
    }
#else
    pFile = fopen(filePath, openMode);
    if (pFile == NULL) {
        goto on_error;
    }
#endif

    if (ppFileOut) *ppFileOut = pFile;
    return OC_RESULT_SUCCESS;

on_error:
    return ocResultFromERRNO(errno);
}

OC_PRIVATE ocResult ocFClose(FILE* pFile)
{
    fclose(pFile);
    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocFSeek(FILE* pFile, ocInt64 offset, int origin)
{
    // TODO: Proper 64-bit seek.
    int result = fseek(pFile, (long)offset, origin);
    if (result != 0) {
        return ocResultFromERRNO(errno);
    }

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocFTell(FILE* pFile, ocUInt64* pPosOut)
{
    // TODO: Proper 64-bit tell.
    long result = ftell(pFile);
    if (result == -1L) {
        return ocResultFromERRNO(errno);
    }

    if (pPosOut) *pPosOut = result;
    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocFRead(FILE* pFile, size_t bytesToRead, void* pData, size_t* pBytesRead)
{
    if (pBytesRead) *pBytesRead = 0;

    // TODO: Proper 64-bit read.
    size_t bytesRead = fread(pData, 1, bytesToRead, pFile);
    if (bytesRead != bytesToRead) {
        if (feof(pFile)) {
            if (pBytesRead) *pBytesRead = bytesRead;
            return OC_RESULT_AT_END_OF_FILE;
        } else {
            return ocResultFromERRNO(errno);
        }
    }

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocFWrite(FILE* pFile, size_t bytesToWrite, const void* pData, size_t* pBytesWritten)
{
    if (pBytesWritten) *pBytesWritten = 0;

    // TODO: Proper 64-bit write.
    size_t bytesWritten = fwrite(pData, 1, bytesToWrite, pFile);
    if (bytesWritten != bytesToWrite) {
        return ocResultFromERRNO(errno);
    }

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE ocResult ocOpenAndReadFileWithExtraData(const char* filePath, void** ppFileData, size_t* pFileSizeOut, size_t extraBytes)
{
    if (pFileSizeOut) *pFileSizeOut = 0;   // For safety.

    if (filePath == NULL) {
        return OC_RESULT_INVALID_ARGS;
    }

    FILE* pFile;
    ocResult result = ocFOpen(filePath, "rb", &pFile);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    ocAssert(pFile != NULL);

    result = ocFSeek(pFile, 0, SEEK_END);
    if (result != OC_RESULT_SUCCESS) {
        ocFClose(pFile);
        return result;
    }

    ocUInt64 fileSize;
    result = ocFTell(pFile, &fileSize);
    if (result != OC_RESULT_SUCCESS) {
        ocFClose(pFile);
        return result;
    }

    result = ocFSeek(pFile, 0, SEEK_SET);
    if (result != OC_RESULT_SUCCESS) {
        ocFClose(pFile);
        return result;
    }

    if ((fileSize + extraBytes > SIZE_MAX) && (fileSize + extraBytes > fileSize)) {
        ocFClose(pFile);
        return OC_RESULT_TOO_LARGE;    // File is too big.
    }

    void* pFileData = ocMalloc((size_t)fileSize + extraBytes);      // <-- Safe cast due to the check above.
    if (pFileData == NULL) {
        ocFClose(pFile);
        return OC_RESULT_OUT_OF_MEMORY;
    }

    result = ocFRead(pFile, (size_t)fileSize, pFileData, NULL);     // <-- Safe cast.
    if (result != OC_RESULT_SUCCESS) {
        ocFree(pFileData);
        ocFClose(pFile);
        return result;
    }

    ocFClose(pFile);

    if (ppFileData) *ppFileData = pFileData;
    if (pFileSizeOut) *pFileSizeOut = (size_t)fileSize;
    return OC_RESULT_SUCCESS;
}

ocResult ocOpenAndReadFile(const char* filePath, void** ppFileData, size_t* pFileSize)
{
    return ocOpenAndReadFileWithExtraData(filePath, ppFileData, pFileSize, 0);
}

ocResult ocOpenAndReadTextFile(const char* filePath, char** ppFileData, size_t* pFileSize)
{
    char* pFileData;
    ocResult result = ocOpenAndReadFileWithExtraData(filePath, (void**)&pFileData, pFileSize, 1);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pFileData[*pFileSize] = '\0';

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        ocFree(pFileData);
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocOpenAndWriteFile(const char* filePath, const void* pFileData, size_t dataSize)
{
    FILE* pFile;
    ocResult result = ocFOpen(filePath, "wb", &pFile);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    if (pFileData != NULL && dataSize > 0) {
        result = ocFWrite(pFile, dataSize, pFileData, NULL);
        if (result != OC_RESULT_SUCCESS) {
            ocFClose(pFile);
            return result;
        }
    }

    ocFClose(pFile);
    return OC_RESULT_SUCCESS;
}

ocResult ocOpenAndWriteTextFile(const char* filePath, const char* pFileData)
{
    if (pFileData == NULL) {
        pFileData = "";
    }

    return ocOpenAndWriteFile(filePath, pFileData, strlen(pFileData));
}
