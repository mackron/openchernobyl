// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocComponentMeshInit(ocWorldObject* pObject, ocComponentMesh* pComponent)
{
    ocResult result = ocComponentInit(pObject, OC_COMPONENT_TYPE_MESH, pComponent);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pComponent->unused = 0;

    return OC_RESULT_SUCCESS;
}

void ocComponentMeshUninit(ocComponentMesh* pComponent)
{
    ocComponentUninit(pComponent);
}