// Copyright (C) 2017 David Reid. See included LICENSE file.

#ifdef OC_WIN32
void ocMakeDPIAware_Win32()
{
    dr_win32_make_dpi_aware();
}
#endif

const char* ocNextToken(const char* tokens, char* tokenOut, size_t tokenOutSize)
{
    if (tokenOut) tokenOut[0] = '\0';

    if (tokens == NULL) {
        return NULL;
    }

    // Skip past leading whitespace.
    while (tokens[0] != '\0' && !(tokens[0] != ' ' && tokens[0] != '\t' && tokens[0] != '\n' && tokens[0] != '\v' && tokens[0] != '\f' && tokens[0] != '\r')) {
        tokens += 1;
    }

    if (tokens[0] == '\0') {
        return NULL;
    }


    const char* strBeg = tokens;
    const char* strEnd = strBeg;

    if (strEnd[0] == '\"')
    {
        // It's double-quoted - loop until the next unescaped quote character.

        // Skip past the first double-quote character.
        strBeg += 1;
        strEnd += 1;

        // Keep looping until the next unescaped double-quote character.
        char prevChar = '\0';
        while (strEnd[0] != '\0' && (strEnd[0] != '\"' || prevChar == '\\'))
        {
            prevChar = strEnd[0];
            strEnd += 1;
        }
    }
    else
    {
        // It's not double-quoted - just loop until the first whitespace.
        while (strEnd[0] != '\0' && (strEnd[0] != ' ' && strEnd[0] != '\t' && strEnd[0] != '\n' && strEnd[0] != '\v' && strEnd[0] != '\f' && strEnd[0] != '\r')) {
            strEnd += 1;
        }
    }


    // If the output buffer is large enough to hold the token, copy the token into it. When we copy the token we need to
    // ensure we don't include the escape character.
    //assert(strEnd >= strBeg);

    while (tokenOutSize > 1 && strBeg < strEnd)
    {
        if (strBeg[0] == '\\' && strBeg[1] == '\"' && strBeg < strEnd) {
            strBeg += 1;
        }

        *tokenOut++ = *strBeg++;
        tokenOutSize -= 1;
    }

    // Null-terminate.
    if (tokenOutSize > 0) {
        *tokenOut = '\0';
    }


    // Skip past the double-quote character before returning.
    if (strEnd[0] == '\"') {
        strEnd += 1;
    }

    return strEnd;
}




///////////////////////////////////////////////////////////////////////////////
//
// Compiler Compatibility
//
///////////////////////////////////////////////////////////////////////////////

#if 0
#ifndef _MSC_VER
int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return ERANGE;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    size_t i;
    for (i = 0; i < dstSizeInBytes && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (i < dstSizeInBytes) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
}

int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return EINVAL;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    size_t maxcount = count;
    if (count == ((size_t)-1) || count >= dstSizeInBytes) {        // -1 = _TRUNCATE
        maxcount = dstSizeInBytes - 1;
    }

    size_t i;
    for (i = 0; i < maxcount && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (src[i] == '\0' || i == count || count == ((size_t)-1)) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
}

int strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return ERANGE;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    char* dstorig = dst;

    while (dstSizeInBytes > 0 && dst[0] != '\0') {
        dst += 1;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        return EINVAL;  // Unterminated.
    }


    while (dstSizeInBytes > 0 && src[0] != '\0') {
        *dst++ = *src++;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return ERANGE;
    }

    return 0;
}

int strncat_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return ERANGE;
    }
    if (src == 0) {
        return EINVAL;
    }

    char* dstorig = dst;

    while (dstSizeInBytes > 0 && dst[0] != '\0') {
        dst += 1;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        return EINVAL;  // Unterminated.
    }


    if (count == ((size_t)-1)) {        // _TRUNCATE
        count = dstSizeInBytes - 1;
    }

    while (dstSizeInBytes > 0 && src[0] != '\0' && count > 0)
    {
        *dst++ = *src++;
        dstSizeInBytes -= 1;
        count -= 1;
    }

    if (dstSizeInBytes > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return ERANGE;
    }

    return 0;
}

int _itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix)
{
    if (dst == NULL || dstSizeInBytes == 0) {
        return EINVAL;
    }
    if (radix < 2 || radix > 36) {
        dst[0] = '\0';
        return EINVAL;
    }

    int sign = (value < 0 && radix == 10) ? -1 : 1;     // The negative sign is only used when the base is 10.

    unsigned int valueU;
    if (value < 0) {
        valueU = -value;
    } else {
        valueU = value;
    }

    char* dstEnd = dst;
    do
    {
        int remainder = valueU % radix;
        if (remainder > 9) {
            *dstEnd = (char)((remainder - 10) + 'a');
        } else {
            *dstEnd = (char)(remainder + '0');
        }

        dstEnd += 1;
        dstSizeInBytes -= 1;
        valueU /= radix;
    } while (dstSizeInBytes > 0 && valueU > 0);

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return EINVAL;  // Ran out of room in the output buffer.
    }

    if (sign < 0) {
        *dstEnd++ = '-';
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return EINVAL;  // Ran out of room in the output buffer.
    }

    *dstEnd = '\0';


    // At this point the string will be reversed.
    dstEnd -= 1;
    while (dst < dstEnd) {
        char temp = *dst;
        *dst = *dstEnd;
        *dstEnd = temp;

        dst += 1;
        dstEnd -= 1;
    }

    return 0;
}
#endif
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Date/Time
//
///////////////////////////////////////////////////////////////////////////////

time_t ocNow()
{
    return time(NULL);
}

void ocDateTimeShort(time_t t, char* strOut, size_t strOutSize)
{
#if defined(_MSC_VER)
	struct tm local;
	localtime_s(&local, &t);
    strftime(strOut, strOutSize, "%x %H:%M:%S", &local);
#else
	struct tm *local = localtime(&t);
	strftime(strOut, strOutSize, "%x %H:%M:%S", local);
#endif
}

void ocDateYYYYMMDD(time_t t, char* strOut, size_t strOutSize)
{
#if defined(_MSC_VER)
	struct tm local;
	localtime_s(&local, &t);
    strftime(strOut, strOutSize, "%Y%m%d", &local);
#else
	struct tm *local = localtime(&t);
	strftime(strOut, strOutSize, "%Y%m%d", local);
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
// Bit Twiddling
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ocBitScanForward32(uint32_t x)
{
#if defined(_MSC_VER)
    unsigned long i;
    if (_BitScanForward(&i, x) == 0) {
        return 0;
    }

    return i+1;
#elif defined(__GNUC__)
    return (unsigned int)__builtin_ffsl((long)x);
#else
    // Fallback.
    if (x == 0) return 0;
    for (int i = 0; i < 32; ++i) {
        if ((x & (1UL << i)) != 0) {
            return i+1;
        }
    }

    return 0;
#endif
}

unsigned int ocBitScanForward64(uint64_t x)
{
#if defined(_MSC_VER) && defined(OC_64BIT)
    unsigned long i;
    if (_BitScanForward64(&i, x) == 0) {
        return 0;
    }

    return i+1;
#elif defined(__GNUC__)
    return (unsigned int)__builtin_ffsll((long long)x);
#else
    // Fallback.
    if (x == 0) return 0;
    for (int i = 0; i < 64; ++i) {
        if ((x & (1ULL << i)) != 0) {
            return i+1;
        }
    }

    return 0;
#endif
}


unsigned int ocBitScanReverse32(uint32_t x)
{
#if defined(_MSC_VER)
    unsigned long i;
    if (_BitScanReverse(&i, x) == 0) {
        return 0;
    }

    return i+1;
#elif defined(__GNUC__)
    if (x == 0) return 0;
    return (unsigned int)(32 - __builtin_clzl(x));
#else
    // Fallback.
    if (x == 0) return 0;
    for (int i = 31; i >= 0; --i) {
        if ((x & (1UL << i)) != 0) {
            return i+1;
        }
    }

    return 0;
#endif
}

unsigned int ocBitScanReverse64(uint64_t x)
{
#if defined(_MSC_VER) && defined(OC_64BIT)
    unsigned long i;
    if (_BitScanReverse64(&i, x) == 0) {
        return 0;
    }

    return i+1;
#elif defined(__GNUC__)
    if (x == 0) return 0;
    return (unsigned int)(64 - __builtin_clzll(x));
#else
    // Fallback.
    if (x == 0) return 0;
    for (int i = 63; i >= 0; --i) {
        if ((x & (1ULL << i)) != 0) {
            return i+1;
        }
    }

    return 0;
#endif
}
