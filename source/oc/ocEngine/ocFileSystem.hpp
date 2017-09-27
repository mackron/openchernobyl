// Copyright (C) 2017 David Reid. See included LICENSE file.

// Access modes. Keep these in sync with dr_fs for simplicity.
#define OC_READ         DRFS_READ
#define OC_WRITE        DRFS_WRITE
#define OC_EXISTING     DRFS_EXISTING
#define OC_TRUNCATE     DRFS_TRUNCATE
#define OC_CREATE_DIRS  DRFS_CREATE_DIRS

// File attributes.
#define OC_FILE_ATTRIBUTE_DIRECTORY DRFS_FILE_ATTRIBUTE_DIRECTORY
#define OC_FILE_ATTRIBUTE_READONLY  DRFS_FILE_ATTRIBUTE_READONLY

enum ocSeekOrigin
{
    ocSeekOrigin_Current,
    ocSeekOrigin_Start,
    ocSeekOrigin_End
};


struct ocFileSystem
{
    drfs_context internalFS;
    ocEngineContext* pEngine;
};

struct ocFile
{
    drfs_file* pInternalFile;
};

typedef struct drfs_file_info ocFileInfo;


// Initializes the file system.
ocResult ocFileSystemInit(ocEngineContext* pEngine, ocFileSystem* pFS);

// Uninitializes the file system.
//
// This does _not_ close any open files. All files should be closed at a higher level. The rationale for this is that
// it simplifies the implementation of the file system.
void ocFileSystemUninit(ocFileSystem* pFS);


// Retrieves information about the given file.
//
// pInfo can be NULL, in which case this function can be used as a simple way of checking whether or not the file exists.
ocResult ocGetFileInfo(ocFileSystem* pFS, const char* relativePath, ocFileInfo* pInfo);

// Finds the absolute path of a file from it's relative path.
//
// This returns an error if the file could not be found.
ocResult ocFindAbsoluteFilePath(ocFileSystem* pFS, const char* relativePath, char* absolutePath, size_t absolutePathSize);


// ocFileOpen
ocResult ocFileOpen(ocFileSystem* pFS, const char* path, unsigned int accessMode, ocFile* pFile);

// ocFileClose
void ocFileClose(ocFile* pFile);

// ocFileRead
ocResult ocFileRead(ocFile* pFile, void* pDataOut, size_t bytesToRead, size_t* pBytesReadOut);

// Writes data to the given file.
ocResult ocFileWrite(ocFile* pFile, const void* pData, size_t bytesToWrite, size_t* pBytesWrittenOut);

// Seeks the file pointer by the given number of bytes, relative to the specified origin.
ocResult ocFileSeek(ocFile* pFile, int64_t bytesToSeek, ocSeekOrigin origin);

// Retrieves the current position of the file pointer.
uint64_t ocFileTell(ocFile* pFile);

// Retrieves the size of the given file.
uint64_t ocFileSize(ocFile* pFile);

// Flushes the given file.
void ocFileFlush(ocFile* pFile);

// Determines whether or not the file is at the end.
ocBool32 ocAtEOF(ocFile* pFile);


///////////////////////////////////////////////////////////////////////////////
//
// High Level File API
//
///////////////////////////////////////////////////////////////////////////////

// Helper function for writing a string.
ocResult ocFileWriteString(ocFile* pFile, const char* str);

// Helper function for writing a string, and then inserting a new line right after it.
//
// The new line character is "\n" and NOT "\r\n".
ocResult ocFileWriteLine(ocFile* pFile, const char* str);



///////////////////////////////////////////////////////////////////////////////
//
// Known Folders and Files
//
///////////////////////////////////////////////////////////////////////////////

// Retrieves the path of the directory that contains the log file.
void ocGetLogFolderPath(ocFileSystem* pFS, char* pathOut, size_t pathOutSize);



///////////////////////////////////////////////////////////////////////////////
//
// General File System APIs
//
///////////////////////////////////////////////////////////////////////////////

// Determines whether or not the given path refers to an actual directory.
ocBool32 ocIsDirectory(const char* path);

// Retrieves the current directory.
//
// The returned string must be freed with ocFreeString().
ocString ocGetCurrentDirectory();

// Sets the current directory of the main file system.
ocResult ocSetCurrentDirectory(const char* path);
