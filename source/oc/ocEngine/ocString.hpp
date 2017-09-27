// Copyright (C) 2017 David Reid. See included LICENSE file.

// Determines whether or not the given string is null or empty.
OC_INLINE ocBool32 ocStringIsNullOrEmpty(const char* str)
{
    return str == NULL || str[0] == '\0';
}

#ifndef _MSC_VER
    #ifndef _TRUNCATE
    #define _TRUNCATE ((size_t)-1)
    #endif
#endif

OC_INLINE char* oc_strcpy(char* dst, const char* src)
{
    if (dst == NULL) return NULL;

    // If the source string is null, just pretend it's an empty string. I don't believe this is standard behaviour of strcpy(), but I prefer it.
    if (src == NULL) {
        src = "\0";
    }

#ifdef _MSC_VER
    while (*dst++ = *src++);
    return dst;
#else
    return strcpy(dst, src);
#endif
}

OC_INLINE int oc_strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
#ifdef _MSC_VER
    return strcpy_s(dst, dstSizeInBytes, src);
#else
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
#endif
}

OC_INLINE int oc_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
#ifdef _MSC_VER
    return strncpy_s(dst, dstSizeInBytes, src, count);
#else
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
#endif
}

OC_INLINE int oc_strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
#ifdef _MSC_VER
    return strcat_s(dst, dstSizeInBytes, src);
#else
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
#endif
}

OC_INLINE int oc_strncat_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
#ifdef _MSC_VER
    return strncat_s(dst, dstSizeInBytes, src, count);
#else
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
#endif
}

OC_INLINE size_t oc_strcpy_len(char* dst, size_t dstSize, const char* src)
{
    if (oc_strcpy_s(dst, dstSize, src) == 0) {
        return strlen(dst);
    }

    return 0;
}

OC_INLINE int oc_stricmp(const char* string1, const char* string2)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    return _stricmp(string1, string2);
#else
    return strcasecmp(string1, string2);
#endif
}


// Converts a UTF-16 character to UTF-32.
OC_INLINE uint32_t ocUTF16ToUTF32ch(ocUInt16 utf16[2])
{
    if (utf16 == NULL) {
        return 0;
    }

    if (utf16[0] < 0xD800 || utf16[0] > 0xDFFF) {
        return utf16[0];
    } else {
        if ((utf16[0] & 0xFC00) == 0xD800 && (utf16[1] & 0xFC00) == 0xDC00) {
            return ((ocUInt32)utf16[0] << 10) + utf16[1] - 0x35FDC00;
        } else {
            return 0;   // Invalid.
        }
    }
}

// Converts a UTF-16 surrogate pair to UTF-32.
OC_INLINE ocUInt32 ocUTF16PairToUTF32ch(ocUInt16 utf160, ocUInt16 utf161)
{
    ocUInt16 utf16[2];
    utf16[0] = utf160;
    utf16[1] = utf161;
    return ocUTF16ToUTF32ch(utf16);
}

// Converts a UTF-32 character to a UTF-16. Returns the number fo UTF-16 values making up the character.
OC_INLINE ocUInt32 ocUTF32ToUTF16ch(ocUInt32 utf32, ocUInt16 utf16[2])
{
    if (utf16 == NULL) {
        return 0;
    }

    if (utf32 < 0xD800 || (utf32 >= 0xE000 && utf32 <= 0xFFFF)) {
        utf16[0] = (ocUInt16)utf32;
        utf16[1] = 0;
        return 1;
    } else {
        if (utf32 >= 0x10000 && utf32 <= 0x10FFFF) {
            utf16[0] = (ocUInt16)(0xD7C0 + (ocUInt16)(utf32 >> 10));
            utf16[1] = (ocUInt16)(0xDC00 + (ocUInt16)(utf32 & 0x3FF));
            return 2;
        } else {
            // Invalid.
            utf16[0] = 0;
            utf16[1] = 0;
            return 0;
        }
    }
}

// Converts a UTF-32 character to a UTF-8 character. Returns the number of bytes making up the UTF-8 character.
OC_INLINE ocUInt32 ocUTF32ToUTF8ch(ocUInt32 utf32, char* utf8, size_t utf8Size)
{
    ocUInt32 utf8ByteCount = 0;
    if (utf32 < 0x80) {
        utf8ByteCount = 1;
    } else if (utf32 < 0x800) {
        utf8ByteCount = 2;
    } else if (utf32 < 0x10000) {
        utf8ByteCount = 3;
    } else if (utf32 < 0x110000) {
        utf8ByteCount = 4;
    }

    if (utf8ByteCount > utf8Size) {
        if (utf8 != NULL && utf8Size > 0) {
            utf8[0] = '\0';
        }
        return 0;
    }

    utf8 += utf8ByteCount;
    if (utf8ByteCount < utf8Size) {
        utf8[0] = '\0'; // Null terminate.
    }

    const unsigned char firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
    switch (utf8ByteCount)
    {
        case 4: *--utf8 = (char)((utf32 | 0x80) & 0xBF); utf32 >>= 6;
        case 3: *--utf8 = (char)((utf32 | 0x80) & 0xBF); utf32 >>= 6;
        case 2: *--utf8 = (char)((utf32 | 0x80) & 0xBF); utf32 >>= 6;
        case 1: *--utf8 = (char)(utf32 | firstByteMark[utf8ByteCount]);
        default: break;
    }

    return utf8ByteCount;
}

OC_INLINE ocBool32 ocIsWhitespace(ocUInt32 utf32)
{
    return utf32 == ' ' || utf32 == '\t' || utf32 == '\n' || utf32 == '\v' || utf32 == '\f' || utf32 == '\r';
}

OC_INLINE const char* ocLTrim(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    while (str[0] != '\0' && !(str[0] != ' ' && str[0] != '\t' && str[0] != '\n' && str[0] != '\v' && str[0] != '\f' && str[0] != '\r')) {
        str += 1;
    }

    return str;
}

OC_INLINE const char* ocRTrim(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    const char* rstr = str;
    while (str[0] != '\0') {
        if (ocIsWhitespace(str[0])) {
            str += 1;
            continue;
        }

        str += 1;
        rstr = str;
    }

    return rstr;
}

OC_INLINE void ocTrim(char* str)
{
    if (str == NULL) {
        return;
    }

    const char* lstr = ocLTrim(str);
    const char* rstr = ocRTrim(lstr);

    if (lstr > str) {
        memmove(str, lstr, rstr-lstr);
    }

    str[rstr-lstr] = '\0';
}



typedef char* ocString;

// Creates a newly allocated string. Free the string with oc_free_string().
ocString ocMakeString(const char* str);

// Creates a formatted string. Free the string with oc_free_string().
ocString ocMakeStringv(const char* format, va_list args);
ocString ocMakeStringf(const char* format, ...);

// Appends a string to another ocString.
//
// This free's "lstr". Use this API like so: "lstr = oc_append_string(lstr, rstr)". It works the same way as realloc().
//
// Use oc_make_stringf("%s%s", str1, str2) to append to C-style strings together. An optimized solution for this may be implemented in the future.
ocString ocAppendString(ocString lstr, const char* rstr);

// Appends a formatted string to another ocString.
ocString ocAppendStringv(ocString lstr, const char* format, va_list args);
ocString ocAppendStringf(ocString lstr, const char* format, ...);


// Frees a string created by oc_make_string*()
void ocFreeString(ocString str);