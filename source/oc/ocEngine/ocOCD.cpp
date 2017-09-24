// Copyright (C) 2017 David Reid. See included LICENSE file.

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

    ocResult result = ocStackInit(&pBuilder->objectStack);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

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

ocResult ocOCDSceneBuilderBeginObject(ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    ocOCDSceneBuilderObject* pNewObject = ocCallocObject(ocOCDSceneBuilderObject);
    if (pNewObject != NULL) {
        return OC_RESULT_OUT_OF_MEMORY;
    }

    ocStackPush(&pBuilder->objectStack, pNewObject);

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderEndObject(ocOCDSceneBuilder* pBuilder)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    ocOCDSceneBuilderObject* pObject;
    ocResult result = ocStackTop(&pBuilder->objectStack, &pObject);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    ocStackPop(&pBuilder->objectStack);

    // TODO: Do something with pObject.

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderSetRelativeTransform(ocOCDSceneBuilder* pBuilder, const glm::vec3 &relativePosition, const glm::quat &relativeRotation, const glm::vec3 &relativeScale)
{
    if (pBuilder == NULL) return OC_RESULT_INVALID_ARGS;

    ocOCDSceneBuilderObject* pObject;
    ocResult result = ocStackTop(&pBuilder->objectStack, &pObject);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pObject->relativePosition = relativePosition;
    pObject->relativeRotation = relativeRotation;
    pObject->relativeScale = relativeScale;

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderAddSceneComponent(ocOCDSceneBuilder* pBuilder)
{
    ocOCDSceneBuilderObject* pObject;
    ocResult result = ocStackTop(&pBuilder->objectStack, &pObject);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // TODO: Do something.

    return OC_RESULT_SUCCESS;
}

ocResult ocOCDSceneBuilderAddMeshComponent(ocOCDSceneBuilder* pBuilder)
{
    ocOCDSceneBuilderObject* pObject;
    ocResult result = ocStackTop(&pBuilder->objectStack, &pObject);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // TODO: Do something.

    return OC_RESULT_SUCCESS;
}