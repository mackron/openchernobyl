// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocComponentMesh : public ocComponent
{
    int unused;
};

//
ocResult ocComponentMeshInit(ocWorldObject* pObject, ocComponentMesh* pComponent);

//
void ocComponentMeshUninit(ocComponentMesh* pComponent);