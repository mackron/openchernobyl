// Copyright (C) 2018 David Reid. See included LICENSE file.

ocBool32 ocPathSegmentsEqual(const char* s0Path, const ocPathSegment s0, const char* s1Path, const ocPathSegment s1)
{
    if (s0Path == NULL || s1Path == NULL) {
        return OC_FALSE;
    }

    if (s0.length != s1.length) {
        return OC_FALSE;
    }

    return strncmp(s0Path + s0.offset, s1Path + s1.offset, s0.length) == 0;
}

ocBool32 ocPathFirst(const char* path, ocPathIterator* i)
{
    if (i == 0) return OC_FALSE;
    i->path = path;
    i->segment.offset = 0;
    i->segment.length = 0;

    if (path == 0 || path[0] == '\0') {
        return OC_FALSE;
    }

    while (i->path[i->segment.length] != '\0' && (i->path[i->segment.length] != '/' && i->path[i->segment.length] != '\\')) {
        i->segment.length += 1;
    }

    return OC_TRUE;
}

ocBool32 ocPathLast(const char* path, ocPathIterator* i)
{
    if (i == 0) return OC_FALSE;
    i->path = path;
    i->segment.offset = 0;
    i->segment.length = 0;

    if (path == 0 || path[0] == '\0') {
        return OC_FALSE;
    }

    i->path = path;
    i->segment.offset = strlen(path);
    i->segment.length = 0;

    return ocPathPrev(i);
}

ocBool32 ocPathNext(ocPathIterator* i)
{
    if (i == NULL || i->path == NULL) {
        return OC_FALSE;
    }

    i->segment.offset = i->segment.offset + i->segment.length;
    i->segment.length = 0;

    while (i->path[i->segment.offset] != '\0' && (i->path[i->segment.offset] == '/' || i->path[i->segment.offset] == '\\')) {
        i->segment.offset += 1;
    }

    if (i->path[i->segment.offset] == '\0') {
        return OC_FALSE;
    }


    while (i->path[i->segment.offset + i->segment.length] != '\0' && (i->path[i->segment.offset + i->segment.length] != '/' && i->path[i->segment.offset + i->segment.length] != '\\')) {
        i->segment.length += 1;
    }

    return OC_TRUE;
}

ocBool32 ocPathPrev(ocPathIterator* i)
{
    if (i == NULL || i->path == NULL || i->segment.offset == 0) {
        return OC_FALSE;
    }

    i->segment.length = 0;

    do
    {
        i->segment.offset -= 1;
    } while (i->segment.offset > 0 && (i->path[i->segment.offset] == '/' || i->path[i->segment.offset] == '\\'));

    if (i->segment.offset == 0) {
        if (i->path[i->segment.offset] == '/' || i->path[i->segment.offset] == '\\') {
            i->segment.length = 0;
            return OC_TRUE;
        }

        return OC_FALSE;
    }


    size_t offsetEnd = i->segment.offset + 1;
    while (i->segment.offset > 0 && (i->path[i->segment.offset] != '/' && i->path[i->segment.offset] != '\\')) {
        i->segment.offset -= 1;
    }

    if (i->path[i->segment.offset] == '/' || i->path[i->segment.offset] == '\\') {
        i->segment.offset += 1;
    }


    i->segment.length = offsetEnd - i->segment.offset;

    return OC_TRUE;
}

ocBool32 ocPathAtEnd(ocPathIterator i)
{
    return i.path == 0 || i.path[i.segment.offset] == '\0';
}

ocBool32 ocPathAtStart(ocPathIterator i)
{
    return i.path != 0 && i.segment.offset == 0;
}

ocBool32 ocPathIteratorsEqual(const ocPathIterator i0, const ocPathIterator i1)
{
    return ocPathSegmentsEqual(i0.path, i0.segment, i1.path, i1.segment);
}


ocBool32 ocPathIsRoot(const char* path)
{
    return ocPathIsUnixStyleRoot(path) || ocPathIsWin32StyleRoot(path);
}

ocBool32 ocPathIsRootSegment(const char* path, const ocPathSegment segment)
{
    return ocPathIsUnixStyleRootSegment(path, segment) || ocPathIsWin32StyleRootSegment(path, segment);
}

ocBool32 ocPathIsUnixStyleRoot(const char* path)
{
    if (path == NULL) {
        return OC_FALSE;
    }

    if (path[0] == '/') {
        return OC_TRUE;
    }

    return OC_FALSE;
}

ocBool32 ocPathIsUnixStyleRootSegment(const char* path, const ocPathSegment segment)
{
    if (path == NULL) {
        return OC_FALSE;
    }

    if (segment.offset == 0 && segment.length == 0) {
        return OC_TRUE;    // "/" style root.
    }

    return OC_FALSE;
}

ocBool32 ocPathIsWin32StyleRoot(const char* path)
{
    if (path == NULL) {
        return OC_FALSE;
    }

    if (((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z')) && path[1] == ':' && path[2] == '\0') {
        return OC_TRUE;
    }

    return OC_FALSE;
}

ocBool32 ocPathIsWin32StyleRootSegment(const char* path, const ocPathSegment segment)
{
    if (path == NULL) {
        return OC_FALSE;
    }

    if (segment.offset == 0 && segment.length == 2) {
        if (((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z')) && path[1] == ':') {
            return OC_TRUE;
        }
    }

    return OC_FALSE;
}




void ocPathToForwardSlashes(char* path)
{
    if (path == NULL) {
        return;
    }

    while (path[0] != '\0') {
        if (path[0] == '\\') {
            path[0] = '/';
        }

        path += 1;
    }
}

void ocPathToBackSlashes(char* path)
{
    if (path == NULL) {
        return;
    }

    while (path[0] != '\0') {
        if (path[0] == '/') {
            path[0] = '\\';
        }

        path += 1;
    }
}


ocBool32 ocPathIsDescendant(const char* descendantAbsolutePath, const char* parentAbsolutePath)
{
    ocPathIterator iChild;
    if (!ocPathFirst(descendantAbsolutePath, &iChild)) {
        return OC_FALSE;   // The descendant is an empty string which makes it impossible for it to be a descendant.
    }

    ocPathIterator iParent;
    if (ocPathFirst(parentAbsolutePath, &iParent)) {
        do {
            // If the segment is different, the paths are different and thus it is not a descendant.
            if (!ocPathIteratorsEqual(iParent, iChild)) {
                return OC_FALSE;
            }

            if (!ocPathNext(&iChild)) {
                return OC_FALSE;   // The descendant is shorter which means it's impossible for it to be a descendant.
            }
        } while (ocPathNext(&iParent));
    }

    return OC_TRUE;
}

ocBool32 ocPathIsChild(const char* childAbsolutePath, const char* parentAbsolutePath)
{
    ocPathIterator iChild;
    if (!ocPathFirst(childAbsolutePath, &iChild)) {
        return OC_FALSE;   // The descendant is an empty string which makes it impossible for it to be a descendant.
    }

    ocPathIterator iParent;
    if (ocPathFirst(parentAbsolutePath, &iParent)) {
        do {
            // If the segment is different, the paths are different and thus it is not a descendant.
            if (!ocPathIteratorsEqual(iParent, iChild)) {
                return OC_FALSE;
            }

            if (!ocPathNext(&iChild)) {
                return OC_FALSE;   // The descendant is shorter which means it's impossible for it to be a descendant.
            }
        } while (ocPathNext(&iParent));
    }

    // At this point we have finished iteration of the parent, which should be shorter one. We now do one more iterations of
    // the child to ensure it is indeed a direct child and not a deeper descendant.
    return !ocPathNext(&iChild);
}


const char* ocPathFileName(const char* path)
{
    if (path == NULL) {
        return NULL;
    }

    const char* fileName = path;

    // We just loop through the path until we find the last slash.
    while (path[0] != '\0') {
        if (path[0] == '/' || path[0] == '\\') {
            fileName = path;
        }

        path += 1;
    }

    // At this point the file name is sitting on a slash, so just move forward.
    while (fileName[0] != '\0' && (fileName[0] == '/' || fileName[0] == '\\')) {
        fileName += 1;
    }

    return fileName;
}

const char* ocPathExtension(const char* path)
{
    if (path == NULL) {
        path = "";
    }

    const char* extension = ocPathFileName(path);
    const char* lastOccurance = NULL;

    // Just find the last '.' and return.
    while (extension[0] != '\0') {
        if (extension[0] == '.') {
            extension += 1;
            lastOccurance = extension;
        }

        extension += 1;
    }

    return (lastOccurance != NULL) ? lastOccurance : extension;
}


ocBool32 ocPathEqual(const char* path1, const char* path2)
{
    if (path1 == NULL || path2 == NULL) {
        return OC_FALSE;
    }

    if (path1 == path2 || (path1[0] == '\0' && path2[0] == '\0')) {
        return OC_TRUE;    // Two empty paths are treated as the same.
    }

    ocPathIterator iPath1;
    ocPathIterator iPath2;
    if (ocPathFirst(path1, &iPath1) && ocPathFirst(path2, &iPath2)) {
        ocBool32 isPath1Valid;
        ocBool32 isPath2Valid;

        do {
            if (!ocPathIteratorsEqual(iPath1, iPath2)) {
                return OC_FALSE;
            }

            isPath1Valid = ocPathNext(&iPath1);
            isPath2Valid = ocPathNext(&iPath2);

        } while (isPath1Valid && isPath2Valid);

        // At this point either iPath1 and/or iPath2 have finished iterating. If both of them are at the end, the two paths are equal.
        return isPath1Valid == isPath2Valid && iPath1.path[iPath1.segment.offset] == '\0' && iPath2.path[iPath2.segment.offset] == '\0';
    }

    return OC_FALSE;
}

ocBool32 ocPathExtensionEqual(const char* path, const char* extension)
{
    if (path == NULL || extension == NULL) {
        return OC_FALSE;
    }

    const char* ext1 = extension;
    const char* ext2 = ocPathExtension(path);

#ifdef _MSC_VER
    return _stricmp(ext1, ext2) == 0;
#else
    return strcasecmp(ext1, ext2) == 0;
#endif
}


ocBool32 ocPathIsRelative(const char* path)
{
    if (path == NULL) {
        return OC_FALSE;
    }

    ocPathIterator seg;
    if (ocPathFirst(path, &seg)) {
        return !ocPathIsRootSegment(seg.path, seg.segment);
    }

    // We'll get here if the path is empty. We consider this to be a relative path.
    return OC_TRUE;
}

ocBool32 ocPathIsAbsolute(const char* path)
{
    return !ocPathIsRelative(path);
}


size_t ocPathBasePath(char* pathOut, size_t pathOutSize, const char* path)
{
    if (pathOut != NULL && pathOutSize > 0) {
        pathOut[0] = '\0';
    }

    if (path == NULL) {
        return 0;
    }

    const char* pathorig = path;
    const char* baseend = path;

    // We just loop through the path until we find the last slash.
    while (path[0] != '\0') {
        if (path[0] == '/' || path[0] == '\\') {
            baseend = path;
        }

        path += 1;
    }

    // Now we just loop backwards until we hit the first non-slash (this handles situations where there may be multiple path separators such as "C:/MyFolder////MyFile").
    while (baseend > path) {
        if (baseend[0] != '/' && baseend[0] != '\\') {
            break;
        }

        baseend -= 1;
    }

    size_t pathOutLen = (baseend - pathorig);
    if (pathOut != NULL) {
        oc_strncpy_s(pathOut, pathOutSize, pathorig, pathOutLen);
    }

    return (baseend - pathorig) + 1;    // +1 for null terminator
}

size_t ocPathFileNameWithoutExtension(char* pathOut, size_t pathOutSize, const char* path)
{
    return ocPathRemoveExtension(pathOut, pathOutSize, ocPathFileName(path));
}


static size_t ocPathAppend_Internal(char* pathOut, size_t pathOutSize, const char* base, const char* other, size_t otherLen)
{
    if (base == NULL) {
        base = "";
    }

    if (other == NULL) {
        other = "";
        otherLen = 0;
    }

    // It only ever makes sense to "append" an absolute path to a blank path. If the other path is absolute, but the base
    // path is not blank we need to return an error because it just doesn't make sense to do this.
    if (ocPathIsAbsolute(other) && base[0] != '\0') {
        return 0;
    }

    if (other[0] == '\0') {
        if (pathOut != NULL) {
            if (pathOut != base) {
                oc_strcpy_s(pathOut, pathOutSize, base);
            }
        }
        return strlen(base) + 1;    // +1 for null terminator.
    }


    size_t path1Length = strlen(base);
    size_t path2Length = otherLen;
    size_t slashLength = 0;

    if (path1Length > 0 && base[path1Length-1] != '/' && base[path1Length-1] != '\\') {
        slashLength = 1;
    }

    size_t pathOutLength = path1Length + slashLength + path2Length;

    if (pathOut != NULL) {
        if (pathOutLength+1 <= pathOutSize) {
            if (pathOut != base) {
                oc_strcpy_s(pathOut + 0, pathOutSize - 0, base);
            }
            oc_strcpy_s(pathOut + path1Length,     pathOutSize - path1Length,     "/");
            oc_strcpy_s(pathOut + path1Length + 1, pathOutSize - path1Length - 1, other);
        }
    }

    return pathOutLength + 1;   // +1 for null terminator.
}

size_t ocPathAppend(char* pathOut, size_t pathOutSize, const char* base, const char* other)
{
    if (other == NULL) {
        other = "";
    }

    return ocPathAppend_Internal(pathOut, pathOutSize, base, other, strlen(other));
}

size_t ocPathAppendIterator(char* pathOut, size_t pathOutSize, const char* base, ocPathIterator i)
{
    return ocPathAppend_Internal(pathOut, pathOutSize, base, i.path + i.segment.offset, i.segment.length);
}

size_t ocPathAppendExtension(char* pathOut, size_t pathOutSize, const char* base, const char* extension)
{
    if (base == NULL) {
        base = "";
    }

    if (extension == NULL) {
        extension = "";
    }

    if (extension[0] == '\0') {
        if (pathOut != NULL) {
            if (pathOut != base) {
                oc_strcpy_s(pathOut, pathOutSize, base);
            }
        }
        return strlen(base) + 1;    // +1 for null terminator.
    }


    size_t baseLength = strlen(base);
    size_t extLength  = strlen(extension);

    size_t pathOutLength = baseLength + 1 + extLength;

    if (pathOut != NULL) {
        if (pathOutLength+1 <= pathOutSize) {
            if (pathOut != base) {
                oc_strcpy_s(pathOut + 0, pathOutSize - 0, base);
            }
            oc_strcpy_s(pathOut + baseLength,     pathOutSize - baseLength,     ".");
            oc_strcpy_s(pathOut + baseLength + 1, pathOutSize - baseLength - 1, extension);
        }
    }

    return pathOutLength + 1;   // +1 for null terminator.
}


size_t ocPathClean_TryWrite(ocPathIterator* iterators, unsigned int iteratorCount, char* pathOut, size_t pathOutSize, unsigned int ignoreCounter)
{
    if (iteratorCount == 0) {
        return 0;
    }

    ocPathIterator isegment = iterators[iteratorCount - 1];


    // If this segment is a ".", we ignore it. If it is a ".." we ignore it and increment "ignoreCount".
    ocBool32 ignoreThisSegment = ignoreCounter > 0 && isegment.segment.length > 0;

    if (isegment.segment.length == 1 && isegment.path[isegment.segment.offset] == '.') {
        // "."
        ignoreThisSegment = OC_TRUE;
    } else {
        if (isegment.segment.length == 2 && isegment.path[isegment.segment.offset] == '.' && isegment.path[isegment.segment.offset + 1] == '.') {
            // ".."
            ignoreThisSegment = OC_TRUE;
            ignoreCounter += 1;
        } else {
            // It's a regular segment, so decrement the ignore counter.
            if (ignoreCounter > 0) {
                ignoreCounter -= 1;
            }
        }
    }


    // The previous segment needs to be written before we can write this one.
    size_t bytesWritten = 0;

    ocPathIterator prev = isegment;
    if (!ocPathPrev(&prev)) {
        if (iteratorCount > 1) {
            iteratorCount -= 1;
            prev = iterators[iteratorCount - 1];
        } else {
            prev.path           = NULL;
            prev.segment.offset = 0;
            prev.segment.length = 0;
        }
    }

    if (prev.segment.length > 0) {
        iterators[iteratorCount - 1] = prev;
        bytesWritten = ocPathClean_TryWrite(iterators, iteratorCount, pathOut, pathOutSize, ignoreCounter);
    }


    if (!ignoreThisSegment) {
        if (pathOut != NULL) {
            pathOut += bytesWritten;
            if (pathOutSize >= bytesWritten) {
                pathOutSize -= bytesWritten;
            } else {
                pathOutSize = 0;
            }
        }

        if (bytesWritten > 0) {
            if (pathOut != NULL) {
                pathOut[0] = '/';
                pathOut += 1;
                if (pathOutSize >= 1) {
                    pathOutSize -= 1;
                } else {
                    pathOutSize = 0;
                }
            }
            bytesWritten += 1;
        }

        if (pathOut != NULL) {
            oc_strncpy_s(pathOut, pathOutSize, isegment.path + isegment.segment.offset, isegment.segment.length);
        }
        bytesWritten += isegment.segment.length;
    }

    return bytesWritten;
}

size_t ocPathClean(char* pathOut, size_t pathOutSize, const char* path)
{
    if (path == NULL) {
        return 0;
    }

    ocPathIterator last;
    if (ocPathLast(path, &last)) {
        size_t bytesWritten = 0;
        if (path[0] == '/') {
            if (pathOut != NULL && pathOutSize > 1) {
                pathOut[0] = '/';
            }
            bytesWritten = 1;
        }

        if (pathOut == NULL || pathOutSize <= bytesWritten) {
            bytesWritten += ocPathClean_TryWrite(&last, 1, NULL, 0, 0);
        } else {
            bytesWritten += ocPathClean_TryWrite(&last, 1, pathOut + bytesWritten, pathOutSize - bytesWritten - 1, 0);  // -1 to ensure there is enough room for a null terminator later on.
        }
        
        if (pathOut != NULL && pathOutSize > bytesWritten) {
            pathOut[bytesWritten] = '\0';
        }

        return bytesWritten + 1;
    }

    return 0;
}

size_t ocPathAppendAndClean(char* pathOut, size_t pathOutSize, const char* base, const char* other)
{
    if (base == NULL || other == NULL) {
        return 0;
    }

    ocPathIterator last[2] = {
        {NULL, {0, 0}},
        {NULL, {0, 0}}
    };

    ocBool32 isPathEmpty0 = !ocPathLast(base,  last + 0);
    ocBool32 isPathEmpty1 = !ocPathLast(other, last + 1);

    int iteratorCount = !isPathEmpty0 + !isPathEmpty1;
    if (iteratorCount == 0) {
        return 0;   // Both input strings are empty.
    }

    size_t bytesWritten = 0;
    if (base[0] == '/') {
        if (pathOut != NULL && pathOutSize > 1) {
            pathOut[0] = '/';
        }
        bytesWritten = 1;
    }

    if (pathOut == NULL || pathOutSize <= bytesWritten) {
        bytesWritten += ocPathClean_TryWrite(last, 2, NULL, 0, 0);
    } else {
        bytesWritten += ocPathClean_TryWrite(last, 2, pathOut + bytesWritten, pathOutSize - bytesWritten - 1, 0);  // -1 to ensure there is enough room for a null terminator later on.
    }

    if (pathOut != NULL && pathOutSize > bytesWritten) {
        pathOut[bytesWritten] = '\0';
    }

    return bytesWritten + 1;
}


size_t ocPathRemoveExtension(char* pathOut, size_t pathOutSize, const char* path)
{
    if (path == NULL) {
        path = "";
    }

    const char* extension = ocPathExtension(path);
    if (extension != NULL && extension != '\0') {
        extension -= 1; // -1 to ensure the dot is removed as well.
    }

    size_t pathOutLength = (size_t)(extension - path);

    if (pathOut != NULL) {
        oc_strncpy_s(pathOut, pathOutSize, path, pathOutLength);
    }

    return pathOutLength;
}

size_t ocPathRemoveFileName(char* pathOut, size_t pathOutSize, const char* path)
{
    if (path == NULL) {
        path = "";
    }

    // We just create an iterator that starts at the last segment. We then move back one and place a null terminator at the end of
    // that segment. That will ensure the resulting path is not left with a slash.
    ocPathIterator iLast;
    if (!ocPathLast(path, &iLast)) {
        return 0;   // The path is empty.
    }

    // Don't remove root segments.
    if (ocPathIsRootSegment(iLast.path, iLast.segment)) {
        return 0;
    }

    // If the last segment (the file name portion of the path) is the only segment, just return an empty string. Otherwise we copy
    // up to the end of the second last segment.
    ocPathIterator iSecondLast = iLast;
    if (ocPathPrev(&iSecondLast)) {
        size_t pathOutLength;
        if (ocPathIsUnixStyleRootSegment(iSecondLast.path, iSecondLast.segment)) {
            pathOutLength = iLast.segment.offset;
        } else {
            pathOutLength = iSecondLast.segment.offset + iSecondLast.segment.length;
        }

        if (pathOut != NULL) {
            oc_strncpy_s(pathOut, pathOutSize, path, pathOutLength);
        }

        return pathOutLength + 1;
    } else {
        if (pathOut != NULL && pathOutSize > 0) {
            pathOut[0] = '\0';
        }

        return 1;   // Return 1 because we need to include the null terminator.
    }
}

size_t ocPathRemoveFileNameInPlace(char* path)
{
    if (path == NULL) {
        return 0;
    }

    // We just create an iterator that starts at the last segment. We then move back one and place a null terminator at the end of
    // that segment. That will ensure the resulting path is not left with a slash.
    ocPathIterator iLast;
    if (!ocPathLast(path, &iLast)) {
        return 0;   // The path is empty.
    }

    // Don't remove root segments.
    if (ocPathIsRootSegment(iLast.path, iLast.segment)) {
        return 0;
    }

    // If the last segment (the file name portion of the path) is the only segment, just return an empty string. Otherwise we copy
    // up to the end of the second last segment.
    ocPathIterator iSecondLast = iLast;
    if (ocPathPrev(&iSecondLast)) {
        size_t pathOutLength;
        if (ocPathIsUnixStyleRootSegment(iSecondLast.path, iSecondLast.segment)) {
            pathOutLength = iLast.segment.offset;
        } else {
            pathOutLength = iSecondLast.segment.offset + iSecondLast.segment.length;
        }

        path[pathOutLength] = '\0';
        return pathOutLength + 1;
    } else {
        path[0] = 0;
        return 1;   // Return 1 because we need to include the null terminator.
    }
}


size_t ocPathToRelative(char* pathOut, size_t pathOutSize, const char* absolutePathToMakeRelative, const char* absolutePathToMakeRelativeTo)
{
    // We do this in two phases. The first phase just iterates past each segment of both the path to convert and the
    // base path until we find two that are not equal. The second phase just adds the appropriate ".." segments.

    if (pathOut != NULL && pathOutSize > 0) {
        pathOut[0] = '\0';
    }

    if (!ocPathIsAbsolute(absolutePathToMakeRelative) || !ocPathIsAbsolute(absolutePathToMakeRelativeTo)) {
        return 0;
    }

    ocPathIterator iPath;
    ocPathIterator iBase;
    ocBool32 isPathEmpty = !ocPathFirst(absolutePathToMakeRelative, &iPath);
    ocBool32 isBaseEmpty = !ocPathFirst(absolutePathToMakeRelativeTo, &iBase);

    if (isPathEmpty && isBaseEmpty) {
        return 0;   // Looks like both paths are empty.
    }


    // Phase 1: Get past the common section.
    int isPathAtEnd = 0;
    int isBaseAtEnd = 0;
    while (!isPathAtEnd && !isBaseAtEnd && ocPathIteratorsEqual(iPath, iBase)) {
        isPathAtEnd = !ocPathNext(&iPath);
        isBaseAtEnd = !ocPathNext(&iBase);
    }

    if (iPath.segment.offset == 0) {
        return 0;   // The path is not relative to the base path.
    }


    // Phase 2: Append ".." segments - one for each remaining segment in the base path.
    size_t pathOutLength = 0;

    if (!ocPathAtEnd(iBase)) {
        do {
            if (pathOutLength == 0) {
                // It's the first segment, so we need to ensure we don't lead with a slash.
                if (pathOut != NULL && pathOutLength+2 < pathOutSize) {
                    pathOut[pathOutLength + 0] = '.';
                    pathOut[pathOutLength + 1] = '.';
                }
                pathOutLength += 2;
            } else {
                // It's not the first segment. Make sure we lead with a slash.
                if (pathOut != NULL && pathOutLength+3 < pathOutSize) {
                    pathOut[pathOutLength + 0] = '/';
                    pathOut[pathOutLength + 1] = '.';
                    pathOut[pathOutLength + 2] = '.';
                }
                pathOutLength += 3;
            }
        } while (ocPathNext(&iBase));
    }


    // Now we just append whatever is left of the main path. We want the path to be clean, so we append segment-by-segment.
    if (!ocPathAtEnd(iPath)) {
        do {
            // Leading slash, if required.
            if (pathOutLength != 0) {
                if (pathOut != NULL && pathOutLength+1 < pathOutSize) {
                    pathOut[pathOutLength] = '/';
                }
                pathOutLength += 1;
            }

            if (pathOut != NULL) {
                oc_strncpy_s(pathOut + pathOutLength, pathOutSize - pathOutLength, iPath.path + iPath.segment.offset, iPath.segment.length);
            }
            pathOutLength += iPath.segment.length;
        } while (ocPathNext(&iPath));
    }


    // Always null terminate.
    if (pathOut != NULL && pathOutLength+1 <= pathOutSize) {
        pathOut[pathOutLength] = '\0';
    }

    return pathOutLength + 1;   // +1 for null terminator.
}

size_t ocPathToAbsolute(char* pathOut, size_t pathOutSize, const char* relativePathToMakeAbsolute, const char* basePath)
{
    return ocPathAppendAndClean(pathOut, pathOutSize, basePath, relativePathToMakeAbsolute);
}


///////////////////////////////////////////////////////////////////////////////
//
// High Level APIs
//
///////////////////////////////////////////////////////////////////////////////

ocString ocPathFileNameWithoutExtensionStr(const char* path)
{
    size_t len = ocPathFileNameWithoutExtension(NULL, 0, path);
    if (len == 0) {
        return NULL;
    }

    ocString str = ocMallocString(len+1);
    if (str == NULL) {
        return NULL;
    }

    ocPathFileNameWithoutExtension(str, len+1, path);
    return str;
}
