// Copyright (C) 2018 David Reid. See included LICENSE file.

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

    return ocOCDDataBlockWritePadding64(pBlock);
}

ocResult ocOCDDataBlockWritePadding64(ocOCDDataBlock* pBlock)
{
    ocUInt32 padding = 0;
    return ocOCDDataBlockWrite(pBlock, &padding, pBlock->dataSize & 0x7, NULL);
}





///////////////////////////////////////////////////////////////////////////////
//
// ocOCDSceneBuilder
//
///////////////////////////////////////////////////////////////////////////////

OC_PRIVATE ocResult ocStreamWriterWriteOCDDataBlock(ocStreamWriter* pWriter, const ocOCDDataBlock &block)
{
    return ocStreamWriterWrite(pWriter, block.pData, block.dataSize, NULL);
}

ocResult ocOCDSceneBuilderInit(ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pBuilder);

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

    ocStackUninit(&pBuilder->objectStack);
    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderRender(ocOCDSceneBuilder* pBuilder, ocStreamWriter* pWriter)
{
    ocAssert(pBuilder != NULL);

    // Everything is written to an in-memory data block because we use a 2-pass algorithm which requires us to both read and write data which in turn means we need
    // access to the main data pointer, which ocOCDDataBlock provides.
    ocOCDDataBlock mainDataBlock;
    ocResult result = ocOCDDataBlockInit(&mainDataBlock);
    if (result != OC_RESULT_SUCCESS) {
        return OC_RESULT_SUCCESS;
    }

    // OCD header.
    ocStreamWriterWrite<ocUInt32>(&mainDataBlock, OC_OCD_FOURCC);
    ocStreamWriterWrite<ocUInt32>(&mainDataBlock, OC_OCD_TYPE_ID_SCENE);

    // SCN1 data (main data).
    //
    // At this point, all data offsets will be relative to the data blocks. We need to do rendering in two passes. The first pass just dumps each block to the main
    // stream, keeping track of their data offsets. The second pass updates each of the relative data offsets to absolute offsets.

    // Pass 1
    // ======

    // Header. Offsets are relative to the main payload chunk and updated in the second pass.
    ocUInt64 headerOffset;
    result = ocStreamWriterTell(&mainDataBlock, &headerOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    ocStreamWriterWrite<ocUInt32>(&mainDataBlock, (ocUInt32)pBuilder->subresources.count);
    ocStreamWriterWrite<ocUInt32>(&mainDataBlock, (ocUInt32)pBuilder->objects.count);
    ocStreamWriterWrite<ocUInt64>(&mainDataBlock, 0);
    ocStreamWriterWrite<ocUInt64>(&mainDataBlock, 0);
    ocStreamWriterWrite<ocUInt64>(&mainDataBlock, 0);      // Payload size. Updated in the second pass when the size is known for real.
    
    // Payload.
    ocUInt64 payloadOffset;
    result = ocStreamWriterTell(&mainDataBlock, &payloadOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // Subresources.
    ocUInt64 subresourcesOffset;
    result = ocStreamWriterTell(&mainDataBlock, &subresourcesOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    {
        result = ocStreamWriterWriteOCDDataBlock(&mainDataBlock, pBuilder->subresourceBlock);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }
    }

    // Objects.
    ocUInt64 objectsOffset;
    result = ocStreamWriterTell(&mainDataBlock, &objectsOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    {
        result = ocStreamWriterWriteOCDDataBlock(&mainDataBlock, pBuilder->objectBlock);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }
    }

    // Components.
    ocUInt64 componentsOffset;
    result = ocStreamWriterTell(&mainDataBlock, &componentsOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    {
        result = ocStreamWriterWriteOCDDataBlock(&mainDataBlock, pBuilder->componentBlock);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }
    }

    // Component Data.
    ocUInt64 componentDataOffset;
    result = ocStreamWriterTell(&mainDataBlock, &componentDataOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    {
        result = ocStreamWriterWriteOCDDataBlock(&mainDataBlock, pBuilder->componentDataBlock);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }
    }

    // Subresource Data.
    ocUInt64 subresourceDataOffset;
    result = ocStreamWriterTell(&mainDataBlock, &subresourceDataOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    {
        result = ocStreamWriterWriteOCDDataBlock(&mainDataBlock, pBuilder->subresourceDataBlock);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }
    }

    // String Data.
    ocUInt64 stringDataOffset;
    result = ocStreamWriterTell(&mainDataBlock, &stringDataOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    {
        result = ocStreamWriterWriteOCDDataBlock(&mainDataBlock, pBuilder->stringDataBlock);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }
    }



    // Pass 2
    // ======

    // Header. Offsets are relative to the main payload chunk.
    result = ocStreamWriterSeek(&mainDataBlock, (ocInt64)(headerOffset + sizeof(ocUInt32) + sizeof(ocUInt32)), ocSeekOrigin_Start);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    ocStreamWriterWrite<ocUInt64>(&mainDataBlock, subresourcesOffset);
    ocStreamWriterWrite<ocUInt64>(&mainDataBlock, objectsOffset);

    ocUInt64 payloadSize;
    result = ocStreamWriterSize(&mainDataBlock, &payloadSize);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }
    ocStreamWriterWrite<ocUInt64>(&mainDataBlock, payloadSize);


    // Subresources.
    ocOCDSceneBuilderSubresource* pSubresources = (ocOCDSceneBuilderSubresource*)ocOffsetPtr(mainDataBlock.pData, (ocSizeT)subresourcesOffset);
    for (ocUInt64 iSubresource = 0; iSubresource < pBuilder->subresources.count; ++iSubresource) {
        pSubresources[iSubresource].pathOffset += stringDataOffset;
        pSubresources[iSubresource].dataOffset += subresourceDataOffset;
    }

    // Objects.
    ocOCDSceneBuilderObject* pObjects = (ocOCDSceneBuilderObject*)ocOffsetPtr(mainDataBlock.pData, (ocSizeT)objectsOffset);
    for (ocUInt64 iObject = 0; iObject < pBuilder->objects.count; ++iObject) {
        pObjects[iObject].nameOffset += stringDataOffset;
        pObjects[iObject].componentsOffset += componentsOffset;
    }

    // Components.
    ocOCDSceneBuilderComponent* pComponents = (ocOCDSceneBuilderComponent*)ocOffsetPtr(mainDataBlock.pData, (ocSizeT)componentsOffset);
    for (ocUInt64 iComponent = 0; iComponent < pBuilder->components.count; ++iComponent) {
        pComponents[iComponent].dataOffset += componentDataOffset;
    }



    // The last thing to do is write our in-memory data block to the main stream.
    result = ocStreamWriterWriteOCDDataBlock(pWriter, mainDataBlock);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderAddSubresource(ocOCDSceneBuilder* pBuilder, const char* path, ocUInt32* pIndex)
{
    if (pBuilder == NULL || path == NULL) return OC_RESULT_INVALID_ARGS;

    // Check if the subresource already exists. If so, reuse it.
    for (ocSizeT iSubresource = 0; iSubresource < pBuilder->subresources.count; ++iSubresource) {
        ocOCDSceneBuilderSubresource* pSubresource = &pBuilder->subresources.pItems[iSubresource];
        if (!(pSubresource->flags & OC_OCD_SCENE_SUBRESOURCE_FLAG_IS_INTERNAL)) {
            const char* existingSubresourcePath = (const char*)ocOffsetPtr(pBuilder->stringDataBlock.pData, (ocSizeT)pSubresource->pathOffset);
            if (strcmp(existingSubresourcePath, path) == 0) {
                if (pIndex) *pIndex = iSubresource;
                return OC_RESULT_SUCCESS;
            }
        }
    }

    // If we get here it means the subresource does not already exist and thus needs to be appended.
    ocOCDSceneBuilderSubresource newSubresource;
    ocZeroObject(&newSubresource);
    
    ocResult result = ocOCDDataBlockWriteString(&pBuilder->stringDataBlock, path, &newSubresource.pathOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    result = ocStackPush(&pBuilder->subresources, newSubresource);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    if (pIndex) *pIndex = pBuilder->subresources.count-1;
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

OC_PRIVATE ocResult ocOCDSceneBuilder_AddComponent(ocOCDSceneBuilder* pBuilder, const ocOCDSceneBuilderComponent &component)
{
    ocAssert(pBuilder != NULL);
    ocAssert(pBuilder->objectStack.count > 0);

    // Important Note:
    //
    // The components of an object are stored in the Components block in tightly packed linear order. There can be no gaps in between the
    // components for any single object. This means that components must be added to an object all at once without any changes to the
    // hierarchy in between those component attachments. For example, this is not allowed:
    //
    // - Add Parent Object
    //   - Add Parent Component 1
    //   - Add Child Object
    //     - Add Child Component 1
    //   - Add Parent Component 2
    //
    // In the above example there will be gaps in between the components of the parent object. We need to do a validation check here to
    // guard against this situation, which should be rare in practice, but still possible. To check this we can simply compare the number
    // of components of the current object to the size of the components buffer to determine whether or not any extra components have been
    // added for different objects.
    ocOCDSceneBuilderObject &object = pBuilder->objects.pItems[pBuilder->objectStack.pItems[pBuilder->objectStack.count-1]];
    
    ocUInt32 componentsInObject = object.componentCount;
    if (componentsInObject > 0) {
        ocUInt64 componentBlockSize;
        ocResult result = ocStreamWriterSize(&pBuilder->componentBlock, &componentBlockSize);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        ocUInt32 componentsInBlock = (ocUInt32)((componentBlockSize - object.componentsOffset) / sizeof(ocOCDSceneBuilderComponent));
        if (componentsInBlock != componentsInObject) {
            // The aforementioned validation check failed.
            ocAssert(OC_FALSE);
            return result;
        }
    }


    // First add the component to the Components data block.
    ocUInt64 componentOffset;
    ocResult result = ocOCDDataBlockWrite(&pBuilder->componentBlock, component, &componentOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    object.componentCount += 1;

    // If it's the first component attached to the object, make sure the offset is set. The way components work is that each object will
    // have each of it's components listed in a tightly packed group.
    if (object.componentCount == 1) {
        object.componentsOffset = componentOffset;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderAddSceneComponent(ocOCDSceneBuilder* pBuilder, const char* path)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    if (pBuilder->objectStack.count == 0) {
        return OC_RESULT_INVALID_OPERATION;
    }

    // The first thing to do is add the subresource. Then we just add the component to the object.
    ocUInt32 subresourceIndex;
    ocResult result = ocOCDSceneBuilderAddSubresource(pBuilder, path, &subresourceIndex);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // The component data is simple for scenes - it's just an index to the subresource followed by 4 bytes of 0 padding.
    ocUInt64 componentDataOffset;
    result = ocStreamWriterTell(&pBuilder->componentDataBlock, &componentDataOffset);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    result = ocOCDDataBlockWrite<ocUInt32>(&pBuilder->componentDataBlock, subresourceIndex);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    result = ocOCDDataBlockWrite<ocUInt32>(&pBuilder->componentDataBlock, 0);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    ocUInt64 componentDataOffsetEnd;
    result = ocStreamWriterTell(&pBuilder->componentDataBlock, &componentDataOffsetEnd);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    
    // Now just attach the component to the object.
    ocOCDSceneBuilderComponent component;
    ocZeroObject(&component);
    component.type       = OC_COMPONENT_TYPE_SCENE;
    component.dataSize   = componentDataOffsetEnd - componentDataOffset;
    component.dataOffset = componentDataOffset;
    result = ocOCDSceneBuilder_AddComponent(pBuilder, component);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

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