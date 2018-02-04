// Copyright (C) 2018 David Reid. See included LICENSE file.

// Simple container implementations. Not fully optimized unless necessary.

//
// Stack
//
template <typename T>
struct ocStack
{
    T* pItems;
    size_t count;
    size_t capacity;
};

template <typename T> ocResult ocStackInit(ocStack<T>* pStack)
{
    if (pStack == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pStack);
    pStack->pItems = NULL;
    pStack->count = 0;
    pStack->capacity = 0;

    return OC_RESULT_SUCCESS;
}

template <typename T> ocResult ocStackUninit(ocStack<T>* pStack)
{
    if (pStack == NULL) return OC_RESULT_INVALID_ARGS;

    ocFree(pStack->pItems);
    return OC_RESULT_SUCCESS;
}

template <typename T> ocResult ocStackPush(ocStack<T>* pStack, const T &item)
{
    if (pStack == NULL) return OC_RESULT_INVALID_ARGS;

    if (pStack->capacity == pStack->count) {
        // Resize.
        size_t newCapacity = (pStack->capacity == 0) ? 4 : pStack->capacity*2;
        T* pNewItems = (T*)ocRealloc(pStack->pItems, sizeof(T) * newCapacity);
        if (pNewItems == NULL) {
            return OC_RESULT_OUT_OF_MEMORY;
        }

        pStack->pItems = pNewItems;
        pStack->capacity = newCapacity;
    }

    pStack->pItems[pStack->count] = item;
    pStack->count += 1;

    return OC_RESULT_SUCCESS;
}

template <typename T> ocResult ocStackPop(ocStack<T>* pStack)
{
    if (pStack == NULL) return OC_RESULT_INVALID_ARGS;
    
    if (pStack->count == 0) {
        return OC_RESULT_INVALID_OPERATION;
    }

    pStack->count -= 1;

    return OC_RESULT_SUCCESS;
}

template <typename T> ocResult ocStackTop(ocStack<T>* pStack, T* pItemOut)
{
    if (pStack == NULL) return OC_RESULT_INVALID_ARGS;

    if (pStack->count == 0) {
        return OC_RESULT_INVALID_OPERATION;
    }

    if (pItemOut) *pItemOut = pStack->pItems[pStack->count-1];
    return OC_RESULT_SUCCESS;
}

template <typename T> ocResult ocStackClear(ocStack<T>* pStack)
{
    if (pStack == NULL) return OC_RESULT_INVALID_ARGS;

    pStack->count = 0;
    return OC_RESULT_SUCCESS;
}
