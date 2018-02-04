// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocWorldObjectInit(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pObject == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pObject);

    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;

    pObject->pWorld   = pWorld;
    pObject->position = glm::vec4(0, 0, 0, 0);
    pObject->rotation = glm::quat(1, 0, 0, 0);
    pObject->scale    = glm::vec4(1, 1, 1, 0);

    return OC_RESULT_SUCCESS;
}

void ocWorldObjectUninit(ocWorldObject* pObject)
{
    if (pObject == NULL) return;
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
    if (pObject == NULL) return OC_FALSE;
    return pObject->isInWorld;
}


ocResult ocWorldObjectSetName(ocWorldObject* pObject, const char* name)
{
    if (pObject == NULL) return OC_FALSE;
    
    pObject->name = ocSetString(pObject->name, name);
    if (pObject->name == NULL) {
        return OC_RESULT_OUT_OF_MEMORY;
    }

    return OC_RESULT_SUCCESS;
}

const char* ocWorldObjectGetName(ocWorldObject* pObject)
{
    if (pObject == NULL) return NULL;
    return pObject->name;
}


ocComponent* ocWorldObjectAddComponent(ocWorldObject* pObject, ocComponentType type)
{
    if (pObject == NULL) return NULL;
    
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
    if (pObject == NULL) return;
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
    if (pObject == NULL) return;

    // Components are removed back to front for efficiency.
    while (pObject->componentCount > 0) {
        ocWorldObjectRemoveComponentByIndex(pObject, pObject->componentCount-1);
    }
}


void ocWorldObjectSetPosition(ocWorldObject* pObject, const glm::vec3 &position)
{
    if (pObject == NULL) return;
    ocWorldObjectSetTransform(pObject, position, pObject->rotation, glm::vec3(pObject->scale));
}

void ocWorldObjectSetRotation(ocWorldObject* pObject, const glm::quat &rotation)
{
    if (pObject == NULL) return;
    ocWorldObjectSetTransform(pObject, pObject->position, rotation, glm::vec3(pObject->scale));
}

void ocWorldObjectSetScale(ocWorldObject* pObject, const glm::vec3 &scale)
{
    if (pObject == NULL) return;
    ocWorldObjectSetTransform(pObject, pObject->position, pObject->rotation, scale);
}

void ocWorldObjectSetTransform(ocWorldObject* pObject, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
{
    if (pObject == NULL) return;

    // The state of the world itself needs to be modified when an object is transformed. Thus, we just
    // redirect this and let the world sort out the actual transformation.
    ocWorldSetObjectTransform(pObject->pWorld, pObject, position, rotation, scale);
}
