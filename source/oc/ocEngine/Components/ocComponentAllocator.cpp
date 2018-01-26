// Copyright (C) 2018 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
// Default Allocators
OC_PRIVATE ocComponent* ocCreateComponent_Mesh(ocEngineContext* pEngine, ocComponentType type, ocWorldObject* pObject, void* pUserData)
{
    (void)pEngine;
    (void)type;
    (void)pUserData;

    ocAssert(type == OC_COMPONENT_TYPE_MESH);

    ocMeshComponent* pComponent = (ocMeshComponent*)ocCalloc(1, sizeof(*pComponent));
    if (pComponent == NULL) {
        return NULL;
    }

    if (ocComponentMeshInit(pObject, pComponent)) {
        ocFree(pComponent);
        return NULL;
    }

    return pComponent;
}

OC_PRIVATE void ocDeleteComponent_Mesh(ocComponent* pComponent, void* pUserData)
{
    (void)pUserData;

    ocComponentMeshUninit(reinterpret_cast<ocMeshComponent*>(pComponent));
    ocFree(pComponent);
}


OC_PRIVATE ocComponent* ocCreateComponent_Light(ocEngineContext* pEngine, ocComponentType type, ocWorldObject* pObject, void* pUserData)
{
    (void)pEngine;
    (void)type;
    (void)pUserData;

    ocAssert(type == OC_COMPONENT_TYPE_MESH);

    ocLightComponent* pComponent = (ocLightComponent*)ocCalloc(1, sizeof(*pComponent));
    if (pComponent == NULL) {
        return NULL;
    }

    if (ocComponentLightInit(pObject, pComponent)) {
        ocFree(pComponent);
        return NULL;
    }

    return pComponent;
}

OC_PRIVATE void ocDeleteComponent_Light(ocComponent* pComponent, void* pUserData)
{
    (void)pUserData;

    ocComponentLightUninit(reinterpret_cast<ocLightComponent*>(pComponent));
    ocFree(pComponent);
}
///////////////////////////////////////////////////////////////////////////////


ocResult ocComponentAllocatorInit(ocEngineContext* pEngine, ocComponentAllocator* pAllocator)
{
    if (pAllocator == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pAllocator);

    if (pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    pAllocator->pEngine = pEngine;
    pAllocator->pAllocators = NULL;

    // Default allocators.
    ocComponentAllocatorRegister(pAllocator, OC_COMPONENT_TYPE_MESH, ocCreateComponent_Mesh, ocDeleteComponent_Mesh, NULL);
    ocComponentAllocatorRegister(pAllocator, OC_COMPONENT_TYPE_LIGHT, ocCreateComponent_Light, ocDeleteComponent_Light, NULL);
    
    return OC_RESULT_SUCCESS;
}

void ocComponentAllocatorUninit(ocComponentAllocator* pAllocator)
{
    if (pAllocator == NULL) return;
    stb_sb_free(pAllocator->pAllocators);
}


ocResult ocComponentAllocatorRegister(ocComponentAllocator* pAllocator, ocComponentType type, ocCreateComponentProc onCreate, ocDeleteComponentProc onDelete, void* pUserData)
{
    if (pAllocator == NULL || onCreate == NULL || onDelete == NULL) return OC_RESULT_INVALID_ARGS;

    // Return an error if an allocator has already been registered for this type.
    for (int i = 0; i < stb_sb_count(pAllocator->pAllocators); ++i) {
        if (pAllocator->pAllocators[i].type == type) {
            return OC_RESULT_INVALID_ARGS;
        }
    }

    ocComponentAllocatorInstance allocator;
    allocator.type = type;
    allocator.onCreate = onCreate;
    allocator.onDelete = onDelete;
    allocator.pUserData = pUserData;
    stb_sb_push(pAllocator->pAllocators, allocator);

    return OC_RESULT_SUCCESS;
}


ocComponent* ocComponentAllocatorCreateComponent(ocComponentAllocator* pAllocator, ocComponentType type, ocWorldObject* pObject)
{
    if (pAllocator == NULL) return NULL;

    for (int i = 0; i < stb_sb_count(pAllocator->pAllocators); ++i) {
        if (pAllocator->pAllocators[i].type == type) {
            return pAllocator->pAllocators[i].onCreate(pAllocator->pEngine, type, pObject, pAllocator->pAllocators[i].pUserData);
        }
    }

    // Couldn't find an allocator.
    return NULL;
}

void ocComponentAllocatorDeleteComponent(ocComponentAllocator* pAllocator, ocComponent* pComponent)
{
    if (pAllocator == NULL || pComponent == NULL) return;

    for (int i = 0; i < stb_sb_count(pAllocator->pAllocators); ++i) {
        if (pAllocator->pAllocators[i].type == pComponent->type) {
            pAllocator->pAllocators[i].onDelete(pComponent, pAllocator->pAllocators[i].pUserData);
            break;
        }
    }
}