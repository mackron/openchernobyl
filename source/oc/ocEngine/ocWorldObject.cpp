// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocWorldObjectInit(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pObject == NULL) {
        return OC_INVALID_ARGS;
    }

    ocZeroObject(pObject);

    if (pWorld == NULL) {
        return OC_INVALID_ARGS;
    }

    pObject->pWorld           = pWorld;
    pObject->absolutePosition = glm::vec4(0, 0, 0, 0);
    pObject->absoluteRotation = glm::quat(1, 0, 0, 0);
    pObject->absoluteScale    = glm::vec4(1, 1, 1, 0);

    return OC_SUCCESS;
}

void ocWorldObjectUninit(ocWorldObject* pObject)
{
    if (pObject == NULL) {
        return;
    }

    ocAssert(pObject->pWorld != NULL);

    // The correct way to uninitialize a world object is to first remove it from the world. If you trigger this assert it means you're trying to uninitialize
    // the object while it's still in the world.
    ocAssert(!ocWorldObjectIsInWorld(pObject));

    // Components need to be removed.
    ocWorldObjectRemoveAllComponents(pObject);

    ocFreeString(pObject->name);
}

ocBool32 ocWorldObjectIsInWorld(ocWorldObject* pObject)
{
    if (pObject == NULL) {
        return OC_FALSE;
    }

    return pObject->isInWorld;
}


ocResult ocWorldObjectSetName(ocWorldObject* pObject, const char* name)
{
    if (pObject == NULL) {
        return OC_FALSE;
    }
    
    pObject->name = ocSetString(pObject->name, name);
    if (pObject->name == NULL) {
        return OC_OUT_OF_MEMORY;
    }

    return OC_SUCCESS;
}

const char* ocWorldObjectGetName(ocWorldObject* pObject)
{
    if (pObject == NULL) {
        return NULL;
    }

    return pObject->name;
}


ocComponent* ocWorldObjectAddComponent(ocWorldObject* pObject, ocComponentType type)
{
    if (pObject == NULL) {
        return NULL;
    }
    
    // Check that we haven't hit the component limit.
    if (pObject->componentCount == ocCountOf(pObject->ppComponents)) {
        return NULL;
    }

    ocComponent* pComponent = ocCreateComponent(pObject->pWorld->pEngine, type, pObject);
    if (pComponent == NULL) {
        return NULL;
    }

    pObject->ppComponents[pObject->componentCount] = pComponent;
    pObject->componentCount += 1;

    return pComponent;
}

void ocWorldObjectRemoveComponentByIndex(ocWorldObject* pObject, uint32_t index)
{
    if (pObject == NULL) {
        return;
    }

    ocAssert(index < pObject->componentCount);  // <-- You've got a logic error if you've triggered this.

    // The component allocator is responsible for uninitialization and deletion.
    ocDeleteComponent(pObject->pWorld->pEngine, pObject->ppComponents[index]);

    // Move everything down a slot.
    for (uint16_t i = (uint16_t)index; i < pObject->componentCount-1; ++i) {
        pObject->ppComponents[i] = pObject->ppComponents[i+1];
    }
}

void ocWorldObjectRemoveAllComponents(ocWorldObject* pObject)
{
    if (pObject == NULL) {
        return;
    }

    // Components are removed back to front for efficiency.
    while (pObject->componentCount > 0) {
        ocWorldObjectRemoveComponentByIndex(pObject, pObject->componentCount-1);
    }
}


void ocWorldObjectSetAbsolutePosition(ocWorldObject* pObject, const glm::vec3 &absolutePosition)
{
    if (pObject == NULL) {
        return;
    }

    ocWorldObjectSetAbsoluteTransform(pObject, absolutePosition, pObject->absoluteRotation, glm::vec3(pObject->absoluteScale));
}

void ocWorldObjectSetAbsoluteRotation(ocWorldObject* pObject, const glm::quat &absoluteRotation)
{
    if (pObject == NULL) {
        return;
    }

    ocWorldObjectSetAbsoluteTransform(pObject, pObject->absolutePosition, absoluteRotation, glm::vec3(pObject->absoluteScale));
}

void ocWorldObjectSetAbsoluteScale(ocWorldObject* pObject, const glm::vec3 &absoluteScale)
{
    if (pObject == NULL) {
        return;
    }

    ocWorldObjectSetAbsoluteTransform(pObject, pObject->absolutePosition, pObject->absoluteRotation, absoluteScale);
}

void ocWorldObjectSetAbsoluteTransform(ocWorldObject* pObject, const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale)
{
    if (pObject == NULL) {
        return;
    }

    // The state of the world itself needs to be modified when an object is transformed. Thus, we just
    // redirect this and let the world sort out the actual transformation.
    ocWorldSetObjectAbsoluteTransform(pObject->pWorld, pObject, absolutePosition, absoluteRotation, absoluteScale);
}
