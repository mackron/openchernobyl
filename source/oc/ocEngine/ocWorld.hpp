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


//
void ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject);

//
void ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject);


// Sets the absolute position, rotation and scale of an object as a single operation. You should rarely need to call this
// directly. Instead you should use ocWorldObjectSetAbsolutePosition(), etc.
void ocWorldSetObjectAbsoluteTransform(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale);