// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocDynamicsWorldInit(ocDynamicsWorld* pWorld)
{
    if (pWorld == NULL) {
        return OC_INVALID_ARGS;
    }

    ocZeroObject(pWorld);

    return OC_SUCCESS;
}

void ocDynamicsWorldUninit(ocDynamicsWorld* pWorld)
{
    if (pWorld == NULL) {
        return;
    }
}


void ocDynamicsWorldStep(ocDynamicsWorld* pWorld, double dt)
{
    if (pWorld == NULL || dt <= 0) {
        return;
    }

    /* Not yet implemented. */
}


void ocDynamicsWorldSetGravity(ocDynamicsWorld* pWorld, const glm::vec3 &gravity)
{
    if (pWorld == NULL) {
        return;
    }

    /* Not yet implemented. */
    (void)gravity;
}

glm::vec3 ocDynamicsWorldGetGravity(ocDynamicsWorld* pWorld)
{
    if (pWorld == NULL) {
        return glm::vec3(0, 0, 0);
    }

    /* Not yet implemented. */
    return glm::vec3(0, 0, 0);
}