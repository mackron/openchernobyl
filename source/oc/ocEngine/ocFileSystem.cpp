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

dr_bool32 ocAtEOF(ocFile* pFile)
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