// Copyright (C) 2017 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
//
// ocOCDDataBlock
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocOCDDataBlockInit(ocOCDDataBlock* pBlock)
{
    if (pBlock == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pBlock);
    ocStreamWriterInit(&pBlock->pData, &pBlock->dataSize, pBlock);

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDDataBlockUninit(ocOCDDataBlock* pBlock)
{
    if (pBlock == NULL) return OC_RESULT_INVALID_ARGS;

    ocFree(pBlock->pData);
    ocStreamWriterUninit(pBlock);

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDDataBlockWrite(ocOCDDataBlock* pBlock, const void* pData, ocSizeT dataSize, ocUInt64* pOffsetOut)
{
    if (pOffsetOut) *pOffsetOut = 0;
    if (pBlock == NULL || pData == NULL) return OC_RESULT_INVALID_ARGS;

    if (pOffsetOut) *pOffsetOut = pBlock->dataSize;

    ocResult result = ocStreamWriterWrite(pBlock, pData, dataSize, NULL);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    
    return result;
}

ocResult ocOCDDataBlockWriteString(ocOCDDataBlock* pBlock, const char* pString, ocUInt64* pOffsetOut)
{
    ocResult result = ocOCDDataBlockWrite(pBlock, pString, strlen(pString)+1, pOffsetOut);  // +1 to include the null terminator.
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    return ocOCDDAtaBlockWritePadding32(pBlock);
}

ocResult ocOCDDAtaBlockWritePadding32(ocOCDDataBlock* pBlock)
{
    ocUInt32 padding = 0;
    return ocOCDDataBlockWrite(pBlock, &padding, pBlock->dataSize & 0x3, NULL);
}





///////////////////////////////////////////////////////////////////////////////
//
// ocOCDSceneBuilder
//
///////////////////////////////////////////////////////////////////////////////

// Outputs the OCD file to the internal stream writer.
OC_PRIVATE ocResult ocOCDSceneBuilder_Render(ocOCDSceneBuilder* pBuilder)
{
    ocAssert(pBuilder != NULL);

    ocStreamWriter* pWriter = pBuilder->pWriter;
    ocAssert(pWriter != NULL);

    ocStreamWriterWrite<ocUInt32>(pWriter, OC_OCD_FOURCC);
    ocStreamWriterWrite<ocUInt32>(pWriter, OC_OCD_TYPE_ID_SCENE);



    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderInit(ocStreamWriter* pWriter, ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder || pWriter == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pBuilder);
    pBuilder->pWriter = pWriter;

    ocStackInit(&pBuilder->objectStack);
    ocStackInit(&pBuilder->subresources);
    ocStackInit(&pBuilder->objects);
    ocStackInit(&pBuilder->components);

    ocOCDDataBlockInit(&pBuilder->subresourceBlock);
    ocOCDDataBlockInit(&pBuilder->objectBlock);
    ocOCDDataBlockInit(&pBuilder->componentBlock);
    ocOCDDataBlockInit(&pBuilder->componentDataBlock);
    ocOCDDataBlockInit(&pBuilder->subresourceDataBlock);
    ocOCDDataBlockInit(&pBuilder->stringDataBlock);

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderUninit(ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    // Generate the final OCD file.
    ocOCDSceneBuilder_Render(pBuilder);

    ocStackUninit(&pBuilder->objectStack);
    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderBeginObject(ocOCDSceneBuilder* pBuilder, const char* name, const glm::vec3 &relativePosition, const glm::quat &relativeRotation, const glm::vec3 &relativeScale)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    ocResult result = OC_RESULT_SUCCESS;

    ocOCDSceneBuilderObject object;
    ocZeroObject(&object);

    ocUInt32 newObjectIndex = (ocUInt32)pBuilder->objects.count;

    // Name.
    result = ocOCDDataBlockWriteString(&pBuilder->stringDataBlock, name, &object.nameOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // Hierarchy.
    object.parentIndex      = OC_SCENE_OBJECT_NONE;
    object.firstChildIndex  = OC_SCENE_OBJECT_NONE;
    object.lastChildIndex   = OC_SCENE_OBJECT_NONE;
    object.prevSiblingIndex = OC_SCENE_OBJECT_NONE;
    object.nextSiblingIndex = OC_SCENE_OBJECT_NONE;

    if (pBuilder->objectStack.count > 0) {
        object.parentIndex = pBuilder->objectStack.pItems[pBuilder->objectStack.count - 1];

        // The new object's previous sibling is the last child of the parent.
        object.prevSiblingIndex = pBuilder->objects.pItems[object.parentIndex].lastChildIndex;

        // The parent's first and last children need to be updated.
        pBuilder->objects.pItems[object.parentIndex].lastChildIndex = newObjectIndex;
        if (pBuilder->objects.pItems[object.parentIndex].firstChildIndex == OC_SCENE_OBJECT_NONE) {
            pBuilder->objects.pItems[object.parentIndex].firstChildIndex = newObjectIndex;
        }
    } else {
        for (ocUInt32 i = pBuilder->objects.count; i > 0; --i) {
            if (pBuilder->objects.pItems[i-1].parentIndex == OC_SCENE_OBJECT_NONE) {
                object.prevSiblingIndex = i-1;
                break;
            }
        }
    }

    if (object.prevSiblingIndex != OC_SCENE_OBJECT_NONE) {
        pBuilder->objects.pItems[object.prevSiblingIndex].nextSiblingIndex = newObjectIndex;
    }



    // Transformation.
    object.relativePosition = relativePosition;
    object.relativeRotation = relativeRotation;
    object.relativeScale = relativeScale;



    // Add the object to the stack.
    result = ocStackPush(&pBuilder->objectStack, newObjectIndex);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // Add the object to the main list.
    result = ocStackPush(&pBuilder->objects, object);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderEndObject(ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    if (pBuilder->objectStack.count == 0) {
        return OC_RESULT_INVALID_OPERATION;
    }

    ocResult result = ocStackPop(&pBuilder->objectStack);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderAddSceneComponent(ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    if (pBuilder->objectStack.count == 0) {
        return OC_RESULT_INVALID_OPERATION;
    }

    // TODO: Do something.

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderAddMeshComponent(ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    if (pBuilder->objectStack.count == 0) {
        return OC_RESULT_INVALID_OPERATION;
    }

    // TODO: Do something.

    return OC_RESULT_SUCCESS;
}