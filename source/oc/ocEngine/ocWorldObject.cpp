// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocWorldObjectInit(ocWorldObject* pObject, ocWorld* pWorld)
{
    if (pObject == NULL || pWorld == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pObject);
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
}

dr_bool32 ocWorldObjectIsInWorld(ocWorldObject* pObject)
{
    if (pObject == NULL) return false;
    return (pObject->flags & OC_WORLD_OBJECT_FLAG_IN_WORLD) != 0;
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