// Copyright (C) 2017 David Reid. See included LICENSE file.

typedef ocResult (* ocStreamReader_OnReadProc)(void* pUserData, void* pDataOut, ocSizeT bytesToRead, ocSizeT* pBytesRead);
typedef ocResult (* ocStreamReader_OnSeekProc)(void* pUserData, ocInt64 bytesToSeek, ocSeekOrigin origin);

struct ocStreamReader
{
    ocStreamReader_OnReadProc onRead;
    ocStreamReader_OnSeekProc onSeek;
    void* pUserData;

    union
    {
        ocFile* pFile;
        struct
        {
            const ocUInt8* pData;
            ocSizeT dataSize;
            ocSizeT currentPos;
        } memory;
    };
};

//
ocResult ocStreamReaderInit(ocFile* pFile, ocStreamReader* pReader);
ocResult ocStreamReaderInit(const void* pData, ocSizeT dataSize, ocStreamReader* pReader);
ocResult ocStreamReaderInit(ocStreamReader_OnReadProc onRead, ocStreamReader_OnSeekProc onSeek, void* pUserData, ocStreamReader* pReader);

//
ocResult ocStreamReaderUninit(ocStreamReader* pReader);

//
ocResult ocStreamReaderRead(ocStreamReader* pReader, void* pDataOut, ocSizeT bytesToRead, ocSizeT* pBytesRead);

//
ocResult ocStreamReaderSeek(ocStreamReader* pReader, ocInt64 bytesToSeek, ocSeekOrigin origin);