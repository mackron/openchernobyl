// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocDynamicsWorld
{
    int _unused;
};

//
ocResult ocDynamicsWorldInit(ocDynamicsWorld* pWorld);

//
void ocDynamicsWorldUninit(ocDynamicsWorld* pWorld);

//
void ocDynamicsWorldStep(ocDynamicsWorld* pWorld, double dt);

//
void ocDynamicsWorldSetGravity(ocDynamicsWorld* pWorld, const glm::vec3 &gravity);

//
glm::vec3 ocDynamicsWorldGetGravity(ocDynamicsWorld* pWorld);