// Copyright (C) 2017 David Reid. See included LICENSE file.

ocString ocMakeString(const char* str)
{
    if (str == NULL) return NULL;
    
    size_t len = strlen(str);
    char* newStr = (char*)ocMalloc(len+1);
    if (newStr == NULL) {
        return NULL;    // Out of memory.
    }

    oc_strcpy_s(newStr, len+1, str);
    return newStr;
}

ocString ocMakeStringv(const char* format, va_list args)
{
    if (format == NULL) format = "";

    va_list args2;
    va_copy(args2, args);

#if defined(_MSC_VER)
    int len = _vscprintf(format, args2);
#else
    int len = vsnprintf(NULL, 0, format, args2);
#endif

    va_end(args2);
    if (len < 0) {
        return NULL;
    }


    char* str = (char*)ocMalloc(len+1);
    if (str == NULL) {
        return NULL;
    }

#if defined(_MSC_VER)
    len = vsprintf_s(str, len+1, format, args);
#else
    len = vsnprintf(str, len+1, format, args);
#endif

    return str;
}

ocString ocMakeStringf(const char* format, ...)
{
    if (format == NULL) format = "";

    va_list args;
    va_start(args, format);

    char* str = ocMakeStringv(format, args);

    va_end(args);
    return str;
}

ocString ocAppendString(ocString lstr, const char* rstr)
{
    if (rstr == NULL) {
        rstr = "";
    }

    if (lstr == NULL) {
        return ocMakeString(rstr);
    }

    size_t lstrLen = strlen(lstr);
    size_t rstrLen = strlen(rstr);
    char* str = (char*)ocRealloc(lstr, lstrLen + rstrLen + 1);
    if (str == NULL) {
        return NULL;
    }

    memcpy(str + lstrLen, rstr, rstrLen);
    str[lstrLen + rstrLen] = '\0';

    return str;
}

ocString ocAppendStringv(ocString lstr, const char* format, va_list args)
{
    ocString rstr = ocMakeStringv(format, args);
    if (rstr == NULL) {
        return NULL;    // Probably out of memory.
    }

    char* str = ocAppendString(lstr, rstr);

    ocFreeString(rstr);
    return str;
}

ocString ocAppendStringf(ocString lstr, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char* str = ocAppendStringv(lstr, format, args);

    va_end(args);
    return str;
}

ocString ocAppendStringLength(ocString lstr, const char* rstr, size_t rstrLen)
{
    if (rstr == NULL) {
        rstr = "";
    }

    if (lstr == NULL) {
        return ocMakeString(rstr);
    }

    size_t lstrLen = ocStringLength(lstr);
    char* str = (char*)ocRealloc(lstr, lstrLen + rstrLen + 1);
    if (str == NULL) {
        return NULL;
    }

    oc_strncat_s(str, lstrLen + rstrLen + 1, rstr, rstrLen);
    str[lstrLen + rstrLen] = '\0';

    return str;
}

size_t ocStringLength(ocString str)
{
    return strlen(str);
}

void ocFreeString(ocString str)
{
    ocFree(str);
}