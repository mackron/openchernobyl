// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocComponentMesh : public ocComponent
{
    int unused;
};

//
ocResult ocComponentMeshInit(ocComponentMesh* pComponent, ocWorldObject* pObject);

//
void ocComponentMeshUninit(ocComponentMesh* pComponent);