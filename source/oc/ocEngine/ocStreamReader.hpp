// Copyright (C) 2018 David Reid. See included LICENSE file.

typedef ocResult (* ocStreamReader_OnReadProc) (void* pUserData, void* pDataOut, ocSizeT bytesToRead, ocSizeT* pBytesRead);
typedef ocResult (* ocStreamReader_OnSeekProc) (void* pUserData, ocInt64 bytesToSeek, ocSeekOrigin origin);
typedef ocResult (* ocStreamReader_OnTellProc) (void* pUserData, ocUInt64* pPos);
typedef ocBool32 (* ocStreamReader_OnAtEndProc)(void* pUserData);

struct ocStreamReader
{
    ocStreamReader_OnReadProc  onRead;
    ocStreamReader_OnSeekProc  onSeek;
    ocStreamReader_OnTellProc  onTell;
    ocStreamReader_OnAtEndProc onAtEnd;
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
ocResult ocStreamReaderInit(ocStreamReader_OnReadProc onRead, ocStreamReader_OnSeekProc onSeek, ocStreamReader_OnTellProc onTell, ocStreamReader_OnAtEndProc onAtEnd, void* pUserData, ocStreamReader* pReader);

//
ocResult ocStreamReaderUninit(ocStreamReader* pReader);

//
ocResult ocStreamReaderRead(ocStreamReader* pReader, void* pDataOut, ocSizeT bytesToRead, ocSizeT* pBytesRead);

//
ocResult ocStreamReaderSeek(ocStreamReader* pReader, ocInt64 bytesToSeek, ocSeekOrigin origin);

//
ocResult ocStreamReaderTell(ocStreamReader* pReader, ocUInt64* pPos);

//
ocResult ocStreamReaderSize(ocStreamReader* pReader, ocUInt64* pSize);

//
ocBool32 ocStreamReaderAtEnd(ocStreamReader* pReader);