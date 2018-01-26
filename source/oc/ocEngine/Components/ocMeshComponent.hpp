// Copyright (C) 2018 David Reid. See included LICENSE file.

#define OC_MESH_COMPONENT(pComponent) ((ocMeshComponent*)pComponent)
struct ocMeshComponent : public ocComponent
{
    ocGraphicsMesh* pMesh;          // Used as the source for the mesh object.
    ocGraphicsObject* pMeshObject;  // Initially set to NULL, and then initialized when the object is added to the world.
};

//
ocResult ocComponentMeshInit(ocWorldObject* pObject, ocMeshComponent* pComponent);

//
void ocComponentMeshUninit(ocMeshComponent* pComponent);

// Sets the mesh to use for the graphical representation.
//
// This will fail if the object is already in the world. To change the mesh of an object dynamically,
// you'll need to first remove the object from the world, call this function, and then re-add the object.
ocResult ocComponentMeshSetMesh(ocMeshComponent* pComponent, ocGraphicsMesh* pMesh);