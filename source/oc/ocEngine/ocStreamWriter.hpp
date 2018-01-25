// Copyright (C) 2017 David Reid. See included LICENSE file.

typedef ocResult (* ocStreamWriter_OnWriteProc) (void* pUserData, const void* pData, ocSizeT bytesToWrite, ocSizeT* pBytesWritten);
typedef ocResult (* ocStreamWriter_OnSeekProc)  (void* pUserData, ocInt64 bytesToSeek, ocSeekOrigin origin);
typedef ocResult (* ocStreamWriter_OnTellProc)  (void* pUserData, ocUInt64* pPos);
typedef ocResult (* ocStreamWriter_OnSizeProc)  (void* pUserData, ocUInt64* pSize);

struct ocStreamWriter
{
    ocStreamWriter_OnWriteProc onWrite;
    ocStreamWriter_OnSeekProc  onSeek;
    ocStreamWriter_OnTellProc  onTell;
    ocStreamWriter_OnSizeProc  onSize;
    void* pUserData;

    union
    {
        ocFile* pFile;
        struct
        {
            void** ppData;
            ocSizeT* pDataSize;

            ocUInt8* pBuffer;
            ocSizeT bufferSize;
            ocSizeT dataSize;
            ocSizeT currentPos;
        } memory;
    };
};

//
ocResult ocStreamWriterInit(ocFile* pFile, ocStreamWriter* pWriter);

// Initializes a writer that outputs data to the given data and size variables.
//
// The writer will allocate data with ocRealloc(). You must free this data with ocFree() when you are done with it. Uninitializing
// the writer will _not_ free the data.
ocResult ocStreamWriterInit(void** ppData, size_t* pDataSize, ocStreamWriter* pWriter);

//
ocResult ocStreamWriterInit(ocStreamWriter_OnWriteProc onWrite, ocStreamWriter_OnSeekProc onSeek, ocStreamWriter_OnTellProc onTell, ocStreamWriter_OnSizeProc onSize, void* pUserData, ocStreamWriter* pWriter);

//
ocResult ocStreamWriterUninit(ocStreamWriter* pWriter);

//
ocResult ocStreamWriterWrite(ocStreamWriter* pWriter, const void* pData, ocSizeT bytesToWrite, ocSizeT* pBytesWritten);
ocResult ocStreamWriterWriteString(ocStreamWriter* pWriter, const char* str);

template <typename T> ocResult ocStreamWriterWrite(ocStreamWriter* pWriter, T value)
{
    return ocStreamWriterWrite(pWriter, &value, sizeof(value), NULL);
}

//
ocResult ocStreamWriterSeek(ocStreamWriter* pWriter, ocInt64 bytesToSeek, ocSeekOrigin origin);

//
ocResult ocStreamWriterTell(ocStreamWriter* pWriter, ocUInt64* pPos);

//
ocResult ocStreamWriterSize(ocStreamWriter* pWriter, ocUInt64* pSize);