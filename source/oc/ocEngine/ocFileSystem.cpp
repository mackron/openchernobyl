// Copyright (C) 2018 David Reid. See included LICENSE file.

OC_PRIVATE ocResult ocToResult(drfs_result result)
{
    switch (result)
    {
        case drfs_success:                  return OC_SUCCESS;
        case drfs_unknown_error:            return OC_ERROR;
        case drfs_invalid_args:             return OC_INVALID_ARGS;
        case drfs_does_not_exist:           return OC_DOES_NOT_EXIST;
        case drfs_already_exists:           return OC_ALREADY_EXISTS;
        case drfs_permission_denied:        return OC_PERMISSION_DENIED;
        case drfs_too_many_open_files:      return OC_TOO_MANY_OPEN_FILES;
        case drfs_no_backend:               return OC_NO_BACKEND;
        case drfs_out_of_memory:            return OC_OUT_OF_MEMORY;
        case drfs_not_in_write_directory:   return OC_FILE_NOT_IN_WRITE_DIRECTORY;
        case drfs_path_too_long:            return OC_PATH_TOO_LONG;
        case drfs_no_space:                 return OC_NO_SPACE;
        case drfs_not_directory:            return OC_NOT_DIRECTORY;
        case drfs_too_large:                return OC_TOO_LARGE;
        case drfs_at_end_of_file:           return OC_AT_END_OF_FILE;
        case drfs_invalid_archive:          return OC_INVALID_ARCHIVE;
        case drfs_negative_seek:            return OC_NEGATIVE_SEEK;
    }

    return OC_ERROR;
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
    if (pFS == NULL) {
        return OC_INVALID_ARGS;
    }

    ocZeroObject(pFS);

    if (pEngine == NULL) {
        return OC_INVALID_ARGS;
    }

    pFS->pEngine = pEngine;

    if (drfs_init(&pFS->internalFS) != drfs_success) {
        return OC_ERROR;
    }

    // The base directories should always be relative to the executable.
    char exePath[OC_MAX_PATH];
    if (ocGetExecutableDirectoryPath(exePath, sizeof(exePath))) {
        char dataPath[OC_MAX_PATH];
        ocPathAppend(dataPath, sizeof(dataPath), exePath, "data");
        drfs_add_base_directory(&pFS->internalFS, dataPath);

        // The executable's directory should be the lowest priority base directory.
        drfs_add_base_directory(&pFS->internalFS, exePath);
    }

    return OC_SUCCESS;
}

void ocFileSystemUninit(ocFileSystem* pFS)
{
    if (pFS == NULL) {
        return;
    }

    drfs_uninit(&pFS->internalFS);
}


ocResult ocGetFileInfo(ocFileSystem* pFS, const char* relativePath, ocFileInfo* pInfo)
{
    if (pInfo != NULL) {
        ocZeroObject(pInfo);
    }

    return ocToResult(drfs_get_file_info(&pFS->internalFS, relativePath, pInfo));
}

ocResult ocFindAbsoluteFilePath(ocFileSystem* pFS, const char* relativePath, char* absolutePath, size_t absolutePathSize)
{
    if (absolutePath == NULL) {
        return OC_INVALID_ARGS;
    }

    if (absolutePathSize > 0) {
        absolutePath[0] = '\0';
    }

    if (pFS == NULL || relativePath == NULL) {
        return OC_INVALID_ARGS;
    }

    return ocToResult(drfs_find_absolute_path(&pFS->internalFS, relativePath, absolutePath, absolutePathSize));
}



ocResult ocFileOpen(ocFileSystem* pFS, const char* path, unsigned int accessMode, ocFile* pFile)
{
    if (pFile == NULL) {
        return OC_INVALID_ARGS;
    }

    ocZeroObject(pFile);

    if (pFS == NULL || path == NULL) {
        return OC_INVALID_ARGS;
    }

    drfs_result result = drfs_open(&pFS->internalFS, path, accessMode, &pFile->pInternalFile);
    if (result != drfs_success) {
        return ocToResult(result);
    }

    return OC_SUCCESS;
}

void ocFileClose(ocFile* pFile)
{
    if (pFile == NULL) {
        return;
    }

    drfs_close(pFile->pInternalFile);
}

ocResult ocFileRead(ocFile* pFile, void* pDataOut, size_t bytesToRead, size_t* pBytesReadOut)
{
    if (pFile == NULL) {
        return OC_INVALID_ARGS;
    }

    return ocToResult(drfs_read(pFile->pInternalFile, pDataOut, bytesToRead, pBytesReadOut));
}

ocResult ocFileWrite(ocFile* pFile, const void* pData, size_t bytesToWrite, size_t* pBytesWrittenOut)
{
    if (pFile == NULL) {
        return OC_INVALID_ARGS;
    }

    return ocToResult(drfs_write(pFile->pInternalFile, pData, bytesToWrite, pBytesWrittenOut));
}

ocResult ocFileSeek(ocFile* pFile, int64_t bytesToSeek, ocSeekOrigin origin)
{
    if (pFile == NULL) {
        return OC_INVALID_ARGS;
    }

    return ocToResult(drfs_seek(pFile->pInternalFile, bytesToSeek, ocToDRFSSeekOrigin(origin)));
}

ocResult ocFileTell(ocFile* pFile, uint64_t* pPos)
{
    if (pFile == NULL || pPos == NULL) {
        return OC_INVALID_ARGS;
    }

    // Currently no error detection with dr_fs.
    *pPos = drfs_tell(pFile->pInternalFile);
    return OC_SUCCESS;
}

ocResult ocFileSize(ocFile* pFile, uint64_t* pSize)
{
    if (pFile == NULL) {
        return OC_INVALID_ARGS;
    }

    // Currently no error detection with dr_fs.
    *pSize = drfs_size(pFile->pInternalFile);
    return OC_SUCCESS;
}

void ocFileFlush(ocFile* pFile)
{
    if (pFile == NULL) {
        return;
    }

    drfs_flush(pFile->pInternalFile);
}

ocBool32 ocAtEOF(ocFile* pFile)
{
    if (pFile == NULL) {
        return false;
    }

    return drfs_eof(pFile->pInternalFile);
}



///////////////////////////////////////////////////////////////////////////////
//
// High Level File API
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocFileWriteString(ocFile* pFile, const char* str)
{
    if (pFile == NULL) {
        return OC_INVALID_ARGS;
    }

    return ocToResult(drfs_write_string(pFile->pInternalFile, str)); 
}

ocResult ocFileWriteLine(ocFile* pFile, const char* str)
{
    if (pFile == NULL) {
        return OC_INVALID_ARGS;
    }

    return ocToResult(drfs_write_line(pFile->pInternalFile, str)); 
}



///////////////////////////////////////////////////////////////////////////////
//
// Known Folders and Files
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocGetExecutablePath(char* pathOut, size_t pathOutSize)
{
    if (pathOut == NULL) {
        return OC_INVALID_ARGS;
    }

#if defined(OC_WIN32)
    if (pathOut == NULL || pathOutSize == 0) {
        return OC_INVALID_ARGS;
    }

    DWORD length = GetModuleFileNameA(NULL, pathOut, (DWORD)pathOutSize);
    if (length == 0) {
        pathOut[0] = '\0';
        return OC_ERROR;    /* TODO: Return proper error code. */
    }

    // Force null termination.
    if (length == pathOutSize) {
        pathOut[length - 1] = '\0';
    }

    // Back slashes need to be normalized to forward.
    while (pathOut[0] != '\0') {
        if (pathOut[0] == '\\') {
            pathOut[0] = '/';
        }
        pathOut += 1;
    }

    return OC_SUCCESS;
#else
    if (pathOut == NULL || pathOutSize == 0) {
        return OC_INVALID_ARGS;
    }

    ssize_t length = readlink("/proc/self/exe", pathOut, pathOutSize);
    if (length == -1) {
        pathOut[0] = '\0';
        return OC_ERROR;    /* TODO: Return proper error code. */
    }

    if ((size_t)length == pathOutSize) {
        pathOut[length - 1] = '\0';
    } else {
        pathOut[length] = '\0';
    }

    return OC_SUCCESS;
#endif
}

ocResult ocGetExecutableDirectoryPath(char* pathOut, size_t pathOutSize)
{
    ocResult result = ocGetExecutablePath(pathOut, pathOutSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    // A null terminator needs to be placed at the last slash.
    char* lastSlash = pathOut;
    while (pathOut[0] != '\0') {
        if (pathOut[0] == '/' || pathOut[0] == '\\') {
            lastSlash = pathOut;
        }
        pathOut += 1;
    }

    lastSlash[0] = '\0';
    return OC_SUCCESS;
}

void ocGetLogFolderPath(ocFileSystem* pFS, char* pathOut, size_t pathOutSize)
{
    if (pathOut == NULL || pathOutSize == 0) {
        return;
    }

    char logFolderPath[DRFS_MAX_PATH];
    ocBool32 useLocalPath = ocIsPortable(pFS->pEngine); // <-- Use the local log path for portable builds.

#if defined(OC_WIN32)
    // The documentation for SHGetFolderPathA() says that the output path should be the size of MAX_PATH. We'll enforce that just to be safe.
    if (pathOutSize >= MAX_PATH) {
        SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, pathOut);
    } else {
        char pathOutTemp[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, pathOutTemp);

        if (strcpy_s(pathOut, pathOutSize, pathOutTemp) != 0) {
            useLocalPath = OC_TRUE;
        }
    }

    // Back slashes need to be normalized to forward.
    while (pathOut[0] != '\0') {
        if (pathOut[0] == '\\') {
            pathOut[0] = '/';
        }
        pathOut += 1;
    }
#else
    const char* configdir = getenv("XDG_CONFIG_HOME");
    if (configdir != NULL) {
        if (strcpy_s(pathOut, pathOutSize, configdir) != 0) {
            useLocalPath = OC_TRUE;
        }
    } else {
        const char* homedir = getenv("HOME");
        if (homedir == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }

        if (homedir != NULL) {
            if (strcpy_s(pathOut, pathOutSize, homedir) == 0) {
                size_t homedirLength = strlen(homedir);
                pathOut     += homedirLength;
                pathOutSize -= homedirLength;

                if (pathOutSize > 0) {
                    pathOut[0] = '/';
                    pathOut     += 1;
                    pathOutSize -= 1;

                    if (strcpy_s(pathOut, pathOutSize, ".config") != 0) {
                        useLocalPath = OC_TRUE;
                    }
                }
            }
        }
    }
#endif

    if (!useLocalPath) {
        // If the log folder path is relative, assume it's relative to the executable.
        if (ocPathIsRelative(logFolderPath)) {
            ocGetExecutableDirectoryPath(pathOut, sizeof(pathOut));
            ocPathAppend(pathOut, pathOutSize, pathOut, logFolderPath); // In-place append.
        } else {
            strcpy_s(pathOut, pathOutSize, logFolderPath);
        }

        // The folder path needs to be namespaced based on the game name.
        ocPathAppend(pathOut, pathOutSize, pathOut, OC_CONFIG_NAME);    // In-place append.
    } else {
        // We're either running the portable version of we've failed to retrieve the per-user log folder path so we'll just fall back to
        // the executable's directory.
        ocGetExecutableDirectoryPath(pathOut, sizeof(pathOut));
        ocPathAppend(pathOut, pathOutSize, pathOut, "var/log");         // In-place append.
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
        return OC_ERROR;
    }

    return OC_SUCCESS;
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
        return OC_ERROR;
    }

    return OC_SUCCESS;
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
    if (directoryPath == NULL) {
        return OC_INVALID_ARGS;
    }

#ifdef OC_WIN32
    if (CreateDirectoryA(directoryPath, NULL) == 0) {
        return OC_ERROR;
    }
    return OC_SUCCESS;
#endif
#ifdef OC_POSIX
    if (mkdir(directoryPath, 0777) != 0) {
        return OC_REUSLT_UNKNOWN_ERROR;
    }
    return OC_SUCCESS;
#endif
}

ocResult ocCreateDirectoryRecursive(const char* directoryPath)
{
    if (directoryPath == NULL || directoryPath[0] == '\0') {
        return OC_INVALID_ARGS;
    }

    // TODO: Don't restrict this to OC_MAX_PATH.
    // All we need to do is iterate over every segment in the path and try creating the directory.
    char runningPath[OC_MAX_PATH];
    ocZeroMemory(runningPath, sizeof(runningPath));

    size_t i = 0;
    for (;;) {
        if (i >= sizeof(runningPath)-1) {
            return OC_PATH_TOO_LONG;   // Path is too long.
        }

        if (directoryPath[0] == '\0' || directoryPath[0] == '/' || directoryPath[0] == '\\') {
            if (runningPath[0] != '\0' && !(runningPath[1] == ':' && runningPath[2] == '\0')) {   // <-- If the running path is empty, it means we're trying to create the root directory.
                if (!ocIsDirectory(runningPath)) {
                    ocResult result = ocCreateDirectory(runningPath);
                    if (result != OC_SUCCESS) {
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

    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocResultFromERRNO(errno_t e)    // TODO: Move this API somewhere more generic.
{
    switch (e) {
        case EACCES: return OC_PERMISSION_DENIED;
        case EEXIST: return OC_ALREADY_EXISTS;
        case EINVAL: return OC_INVALID_ARGS;
        case EMFILE: return OC_TOO_MANY_OPEN_FILES;
        case ENOENT: return OC_DOES_NOT_EXIST;
        case ENOMEM: return OC_OUT_OF_MEMORY;
        default: break;
    }

    return OC_ERROR;
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
    return OC_SUCCESS;

on_error:
    return ocResultFromERRNO(errno);
}

OC_PRIVATE ocResult ocFClose(FILE* pFile)
{
    fclose(pFile);
    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocFSeek(FILE* pFile, ocInt64 offset, int origin)
{
    // TODO: Proper 64-bit seek.
    int result = fseek(pFile, (long)offset, origin);
    if (result != 0) {
        return ocResultFromERRNO(errno);
    }

    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocFTell(FILE* pFile, ocUInt64* pPosOut)
{
    // TODO: Proper 64-bit tell.
    long result = ftell(pFile);
    if (result == -1L) {
        return ocResultFromERRNO(errno);
    }

    if (pPosOut) *pPosOut = result;
    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocFRead(FILE* pFile, size_t bytesToRead, void* pData, size_t* pBytesRead)
{
    if (pBytesRead) {
        *pBytesRead = 0;
    }

    // TODO: Proper 64-bit read.
    size_t bytesRead = fread(pData, 1, bytesToRead, pFile);
    if (bytesRead != bytesToRead) {
        if (feof(pFile)) {
            if (pBytesRead) *pBytesRead = bytesRead;
            return OC_AT_END_OF_FILE;
        } else {
            return ocResultFromERRNO(errno);
        }
    }

    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocFWrite(FILE* pFile, size_t bytesToWrite, const void* pData, size_t* pBytesWritten)
{
    if (pBytesWritten) {
        *pBytesWritten = 0;
    }

    // TODO: Proper 64-bit write.
    size_t bytesWritten = fwrite(pData, 1, bytesToWrite, pFile);
    if (bytesWritten != bytesToWrite) {
        return ocResultFromERRNO(errno);
    }

    return OC_SUCCESS;
}

OC_PRIVATE ocResult ocOpenAndReadFileWithExtraData(const char* filePath, void** ppFileData, size_t* pFileSizeOut, size_t extraBytes)
{
    if (pFileSizeOut) {
        *pFileSizeOut = 0;   // For safety.
    }

    if (filePath == NULL) {
        return OC_INVALID_ARGS;
    }

    FILE* pFile;
    ocResult result = ocFOpen(filePath, "rb", &pFile);
    if (result != OC_SUCCESS) {
        return result;
    }

    ocAssert(pFile != NULL);

    result = ocFSeek(pFile, 0, SEEK_END);
    if (result != OC_SUCCESS) {
        ocFClose(pFile);
        return result;
    }

    ocUInt64 fileSize;
    result = ocFTell(pFile, &fileSize);
    if (result != OC_SUCCESS) {
        ocFClose(pFile);
        return result;
    }

    result = ocFSeek(pFile, 0, SEEK_SET);
    if (result != OC_SUCCESS) {
        ocFClose(pFile);
        return result;
    }

    if ((fileSize + extraBytes > SIZE_MAX) && (fileSize + extraBytes > fileSize)) {
        ocFClose(pFile);
        return OC_TOO_LARGE;    // File is too big.
    }

    void* pFileData = ocMalloc((size_t)fileSize + extraBytes);      // <-- Safe cast due to the check above.
    if (pFileData == NULL) {
        ocFClose(pFile);
        return OC_OUT_OF_MEMORY;
    }

    result = ocFRead(pFile, (size_t)fileSize, pFileData, NULL);     // <-- Safe cast.
    if (result != OC_SUCCESS) {
        ocFree(pFileData);
        ocFClose(pFile);
        return result;
    }

    ocFClose(pFile);

    if (ppFileData) *ppFileData = pFileData;
    if (pFileSizeOut) *pFileSizeOut = (size_t)fileSize;
    return OC_SUCCESS;
}

ocResult ocOpenAndReadFile(const char* filePath, void** ppFileData, size_t* pFileSize)
{
    return ocOpenAndReadFileWithExtraData(filePath, ppFileData, pFileSize, 0);
}

ocResult ocOpenAndReadTextFile(const char* filePath, char** ppFileData, size_t* pFileSize)
{
    char* pFileData;
    size_t fileSize;
    ocResult result = ocOpenAndReadFileWithExtraData(filePath, (void**)&pFileData, &fileSize, 1);
    if (result != OC_SUCCESS) {
        return result;
    }

    pFileData[fileSize] = '\0';

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        ocFree(pFileData);
    }

    if (pFileSize) {
        *pFileSize = fileSize;
    }

    return OC_SUCCESS;
}

ocResult ocOpenAndWriteFile(const char* filePath, const void* pFileData, size_t dataSize)
{
    FILE* pFile;
    ocResult result = ocFOpen(filePath, "wb", &pFile);
    if (result != OC_SUCCESS) {
        return result;
    }

    if (pFileData != NULL && dataSize > 0) {
        result = ocFWrite(pFile, dataSize, pFileData, NULL);
        if (result != OC_SUCCESS) {
            ocFClose(pFile);
            return result;
        }
    }

    ocFClose(pFile);
    return OC_SUCCESS;
}

ocResult ocOpenAndWriteTextFile(const char* filePath, const char* pFileData)
{
    if (pFileData == NULL) {
        pFileData = "";
    }

    return ocOpenAndWriteFile(filePath, pFileData, strlen(pFileData));
}

ocResult ocCopyFile(const char* srcPath, const char* dstPath, ocBool32 failIfExists)
{
    if (srcPath == NULL || dstPath == NULL) {
        return OC_INVALID_ARGS;
    }

#if _WIN32
    return CopyFileA(srcPath, dstPath, failIfExists) != 0;
#else
    int fdSrc = open(srcPath, O_RDONLY, 0666);
    if (fdSrc == -1) {
        return OC_ERROR;    /* TODO: Return proper error code. Does this set errno? */
    }

    int fdDst = open(dstPath, O_WRONLY | O_TRUNC | O_CREAT | ((failIfExists) ? O_EXCL : 0), 0666);
    if (fdDst == -1) {
        close(fdSrc);
        return OC_ERROR;    /* TODO: Return proper error code. */
    }

    dr_bool32 result = DR_TRUE;
    struct stat info;
    if (fstat(fdSrc, &info) == 0) {
        char buffer[BUFSIZ];
        int bytesRead;
        while ((bytesRead = read(fdSrc, buffer, sizeof(buffer))) > 0) {
            if (write(fdDst, buffer, bytesRead) != bytesRead) {
                result = OC_ERROR;  /* TODO: Return proper error code. */
                break;
            }
        }
    } else {
        result = OC_ERROR;  /* TODO: Return proper error code. */
    }

    close(fdDst);
    close(fdSrc);

    // Permissions.
    chmod(dstPath, info.st_mode & 07777);

    return result;
#endif
}
