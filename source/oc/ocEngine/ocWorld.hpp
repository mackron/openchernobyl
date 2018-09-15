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
void ocWorldDeleteRT(ocWorld* pWorld);


//
void ocWorldStep(ocWorld* pWorld, double dt);

// Draws the world, but does _not_ present it to the game windows. Window presentation needs to be done at a higher level.
void ocWorldDraw(ocWorld* pWorld);


//
void ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject);

//
void ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject);

// Sets the absolute position of the given object.
//
// Use ocWorldSetObjectAbsoluteTransform() if you want to change the position, rotation and/or scale at the same time.
void ocWorldSetObjectAbsolutePosition(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &absolutePosition);

// Sets the absolute rotation of the given object.
//
// Use ocWorldSetObjectAbsoluteTransform() if you want to change the position, rotation and/or scale at the same time.
void ocWorldSetObjectAbsoluteRotation(ocWorld* pWorld, ocWorldObject* pObject, const glm::quat &absoluteRotation);

// Sets the absolute scale of the given object.
//
// Use ocWorldSetObjectAbsoluteTransform() if you want to change the position, rotation and/or scale at the same time.
void ocWorldSetObjectAbsoluteScale(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &absoluteScale);

// Sets the absolute position, rotation and scale of an object as a single operation.
void ocWorldSetObjectAbsoluteTransform(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale);