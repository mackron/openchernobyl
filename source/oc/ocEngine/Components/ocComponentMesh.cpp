// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocComponentMeshInit(ocComponentMesh* pComponent, ocWorldObject* pObject)
{
    ocResult result = ocComponentInit(pComponent, pObject, OC_COMPONENT_TYPE_MESH);
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