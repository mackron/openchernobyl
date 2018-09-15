// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocWorld
{
    ocEngineContext* pEngine;
    ocGraphicsWorld graphicsWorld;
    ocAudioWorld audioWorld;
    ocDynamicsWorld dynamicsWorld;
};

//
ocResult ocWorldInit(ocEngineContext* pEngine, ocWorld* pWorld);

//
void ocWorldUninit(ocWorld* pWorld);


//
ocResult ocWorldCreateRTFromSwapchain(ocWorld* pWorld, ocGraphicsSwapchain* pSwapchain, ocGraphicsRT** ppRT);

//
void ocWorldDeleteRT(ocWorld* pWorld, ocGraphicsRT* pRT);


//
void ocWorldStep(ocWorld* pWorld, double dt);

// Draws the world, but does _not_ present it to the game windows. Window presentation needs to be done at a higher level.
void ocWorldDraw(ocWorld* pWorld);


// Creates a hierarchy of world objects from a resource.
//
// This is a helper API. You do not need to use this to load resources, but it greatly simplifies the process. If you want
// control over memory allocation you may prefer using ocWorldInsertObject() instead.
//
// When using this function, you should not change the object hierarchy as this will cause ocWorldDeleteObject() to not
// work correctly.
//
// This will recursively load relevant sub-resources via the pResourceLibrary object.
ocResult ocWorldCreateObjectFromResource(ocWorld* pWorld, ocResource* pResource, ocResourceLibrary* pResourceLibrary, ocWorldObject** ppObject);

// Recursively deletes the given object.
//
// This will remove the object from the world, uninitialize each one, and then free the memory that was previously
// allocated by ocWorld. This should be used for root objects returned by the ocWorldCreateObject*() APIs. You should
// be careful when using this API because it will recursively uninitialize all children.
void ocWorldDeleteObject(ocWorld* pWorld, ocWorldObject* pObject);

//
ocResult ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject);

//
ocResult ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject);


// Sets the absolute position, rotation and scale of an object as a single operation. You should rarely need to call this
// directly. Instead you should use ocWorldObjectSetAbsolutePosition(), etc.
void ocWorldSetObjectAbsoluteTransform(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale);