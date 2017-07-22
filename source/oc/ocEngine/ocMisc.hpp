// Copyright (C) 2017 David Reid. See included LICENSE file.

#ifdef _MSC_VER
#define OC_INLINE static __inline
#else
#define OC_INLINE static inline
#endif


#ifdef OC_WIN32
// Win32 Only: Makes the application DPI aware.
void ocMakeDPIAware_Win32();
#endif

const char* ocNextToken(const char* tokens, char* tokenOut, size_t tokenOutSize);


///////////////////////////////////////////////////////////////////////////////
//
// Compiler Compatibility
//
///////////////////////////////////////////////////////////////////////////////

// *_s() string APIs. These are need to be implemented for non-MSVC compilers.
#if 0
#ifndef _MSC_VER
#ifndef _TRUNCATE
#define _TRUNCATE ((size_t)-1)
#endif

int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src);
int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count);
int strcat_s(char* dst, size_t dstSizeInBytes, const char* src);
int strncat_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count);
int _itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix);

#ifndef __MINGW32__
OC_INLINE int _stricmp(const char* string1, const char* string2)
{
    return strcasecmp(string1, string2);
}
#endif
#endif
#endif


///////////////////////////////////////////////////////////////////////////////
//
// min/max/clamp
//
///////////////////////////////////////////////////////////////////////////////

#define ocMin(a, b)         (((a) < (b)) ? (a) : (b))
#define ocMax(a, b)         (((a) > (b)) ? (a) : (b))
#define ocClamp(x, lo, hi)  (ocMax(lo, ocMin(x, hi)))



///////////////////////////////////////////////////////////////////////////////
//
// Date/Time
//
///////////////////////////////////////////////////////////////////////////////

// Retrieves the current time.
time_t ocNow();

// Formats a data/time string.
void ocDateTimeShort(time_t t, char* strOut, size_t strOutSize);

// Returns a date string in YYYYMMDD format.
void ocDateYYYYMMDD(time_t t, char* strOut, size_t strOutSize);



///////////////////////////////////////////////////////////////////////////////
//
// Bit Twiddling
//
///////////////////////////////////////////////////////////////////////////////

// Finds the 1-based index of the first set bit, starting from the LSB. Returns 0 if the input is 0.
unsigned int ocBitScanForward32(uint32_t x);
unsigned int ocBitScanForward64(uint64_t x);

// Finds the 1-based index of the first set bit, starting from the MSB. Returns 0 if the input is 0.
unsigned int ocBitScanReverse32(uint32_t x);
unsigned int ocBitScanReverse64(uint64_t x);


// Creates a mask for the most significant set bit.
OC_INLINE uint32_t ocMaskMSB32(uint32_t x)
{
    if (x == 0) return 0;
    return 1UL << (ocBitScanReverse32(x) - 1);
}
OC_INLINE uint64_t ocMaskMSB64(uint64_t x)
{
    if (x == 0) return 0;
    return 1ULL << (ocBitScanReverse64(x) - 1);
}

// Creates a mask for least significant set bit.
OC_INLINE uint32_t ocMaskLSB32(uint32_t x)
{
    if (x == 0) return 0;
    return 1UL << (ocBitScanForward32(x) - 1);
}
OC_INLINE uint64_t ocMaskLSB64(uint64_t x)
{
    if (x == 0) return 0;
    return 1ULL << (ocBitScanForward64(x) - 1);
}


///////////////////////////////////////////////////////////////////////////////
//
// Atomics
//
///////////////////////////////////////////////////////////////////////////////
#if defined(OC_WIN32) && defined(_MSC_VER)
#define ocAtomicIncrement(a) InterlockedIncrement((LONG*)a)
#define ocAtomicDecrement(a) InterlockedDecrement((LONG*)a)
#else
#define ocAtomicIncrement(a) __sync_add_and_fetch(a, 1)
#define ocAtomicDecrement(a) __sync_sub_and_fetch(a, 1)
#endif