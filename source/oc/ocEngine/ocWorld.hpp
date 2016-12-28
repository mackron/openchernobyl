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
void ocWorldStep(ocWorld* pWorld, double dt);

// Draws the world, but does _not_ present it to the game windows. Window presentation needs to be done at a higher level.
void ocWorldDraw(ocWorld* pWorld);


//
void ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject);

//
void ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject);