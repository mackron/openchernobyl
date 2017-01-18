// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocComponentMeshInit(ocWorldObject* pObject, ocComponentMesh* pComponent)
{
    ocResult result = ocComponentInit(pObject, OC_COMPONENT_TYPE_MESH, pComponent);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    return OC_RESULT_SUCCESS;
}

void ocComponentMeshUninit(ocComponentMesh* pComponent)
{
    ocComponentUninit(pComponent);
}

ocResult ocComponentMeshSetMesh(ocComponentMesh* pComponent, ocGraphicsMesh* pMesh)
{
    if (pComponent == NULL) return OC_RESULT_INVALID_ARGS;

    if (ocWorldObjectIsInWorld(pComponent->pObject)) {
        return OC_RESULT_INVALID_OPERATION;
    }

    pComponent->pMesh = pMesh;

    return OC_RESULT_SUCCESS;
}