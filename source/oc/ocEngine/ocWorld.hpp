// Copyright (C) 2016 David Reid. See included LICENSE file.

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
void ocWorldStep(ocWorld* pWorld, double dt);

// Draws the world, but does _not_ present it to the game windows. Window presentation needs to be done at a higher level.
void ocWorldDraw(ocWorld* pWorld);


//
void ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject);

//
void ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject);

// Sets the position of the given object.
//
// Use ocWorldSetObjectTransform() if you want to change the position, rotation and/or scale at the same time.
void ocWorldSetObjectPosition(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &position);

// Sets the rotation of the given object.
//
// Use ocWorldSetObjectTransform() if you want to change the position, rotation and/or scale at the same time.
void ocWorldSetObjectRotation(ocWorld* pWorld, ocWorldObject* pObject, const glm::quat &rotation);

// Sets the scale of the given object.
//
// Use ocWorldSetObjectTransform() if you want to change the position, rotation and/or scale at the same time.
void ocWorldSetObjectScale(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &scale);

// Sets the position, rotation and scale of an object as a single operation.
void ocWorldSetObjectTransform(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale);