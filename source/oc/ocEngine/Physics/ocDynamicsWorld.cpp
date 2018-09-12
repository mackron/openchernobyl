// Copyright (C) 2018 David Reid. See included LICENSE file.

OC_INLINE glm::vec3 ocToVec3(const btVector3 &v)
{
    return glm::vec3(v.x(), v.y(), v.z());
}

OC_INLINE btVector3 ocToBulletVector3(const glm::vec3 &v)
{
    return btVector3(v.x, v.y, v.z);
}


ocResult ocDynamicsWorldInit(ocDynamicsWorld* pWorld)
{
    if (pWorld == NULL) return OC_INVALID_ARGS;

    ocZeroObject(pWorld);
    pWorld->pBroadphase = new btDbvtBroadphase;
    pWorld->pCollisionConfiguration = new btDefaultCollisionConfiguration;
    pWorld->pCollisionDispatcher = new btCollisionDispatcher(pWorld->pCollisionConfiguration);
    pWorld->pSolver = new btSequentialImpulseConstraintSolver;
    pWorld->pInternalWorld = new btDiscreteDynamicsWorld(pWorld->pCollisionDispatcher, pWorld->pBroadphase, pWorld->pSolver, pWorld->pCollisionConfiguration);

    return OC_SUCCESS;
}

void ocDynamicsWorldUninit(ocDynamicsWorld* pWorld)
{
    if (pWorld == NULL) return;

    delete pWorld->pInternalWorld;
    delete pWorld->pSolver;
    delete pWorld->pCollisionDispatcher;
    delete pWorld->pCollisionConfiguration;
    delete pWorld->pBroadphase;
}


void ocDynamicsWorldStep(ocDynamicsWorld* pWorld, double dt)
{
    if (pWorld == NULL || dt <= 0) return;
    pWorld->pInternalWorld->stepSimulation(btScalar(dt));
}


void ocDynamicsWorldSetGravity(ocDynamicsWorld* pWorld, const glm::vec3 &gravity)
{
    if (pWorld == NULL) return;
    pWorld->pInternalWorld->setGravity(ocToBulletVector3(gravity));
}

glm::vec3 ocDynamicsWorldGetGravity(ocDynamicsWorld* pWorld)
{
    if (pWorld == NULL) return glm::vec3(0, 0, 0);
    return ocToVec3(pWorld->pInternalWorld->getGravity());
}