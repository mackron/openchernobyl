// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocWorldInit(ocWorld* pWorld, ocEngineContext* pEngine)
{
    if (pWorld == NULL || pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pWorld);

    // Graphics.
    ocResult result = ocGraphicsWorldInit(&pWorld->graphicsWorld, &pEngine->graphics);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // Audio.
    result = ocAudioWorldInit(&pWorld->audioWorld, &pEngine->audio);
    if (result != OC_RESULT_SUCCESS) {
        ocGraphicsWorldUninit(&pWorld->graphicsWorld);
        return result;
    }

    // Physics.
    result = ocDynamicsWorldInit(&pWorld->dynamicsWorld);
    if (result != OC_RESULT_SUCCESS) {
        ocAudioWorldUninit(&pWorld->audioWorld);
        ocGraphicsWorldUninit(&pWorld->graphicsWorld);
        return result;
    }

    return OC_RESULT_SUCCESS;
}

void ocWorldUninit(ocWorld* pWorld)
{
    if (pWorld == NULL) return;

    ocDynamicsWorldUninit(&pWorld->dynamicsWorld);
    ocAudioWorldUninit(&pWorld->audioWorld);
    ocGraphicsWorldUninit(&pWorld->graphicsWorld);
}


void ocWorldStep(ocWorld* pWorld, double dt)
{
    if (pWorld == NULL) return;
    
    ocDynamicsWorldStep(&pWorld->dynamicsWorld, dt);
    ocGraphicsWorldStep(&pWorld->graphicsWorld, dt);
}

void ocWorldDraw(ocWorld* pWorld)
{
    if (pWorld == NULL) return;

    ocGraphicsWorldDraw(&pWorld->graphicsWorld);
}


void ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) return;

    // If you trigger this assert it means you've mismatched your world and object.
    ocAssert(pObject->pWorld == pWorld);

    // We need to go over each component and create the relevant objects for the the sub-worlds. The connections between the
    // object and the sub-worlds are achieved with an attribute in each component.
    for (uint16_t iComponent = 0; iComponent < pObject->componentCount; ++iComponent) {
        switch (pObject->ppComponents[iComponent]->type)
        {
            case OC_COMPONENT_TYPE_MESH:
            {
            } break;

            case OC_COMPONENT_TYPE_PARTICLE_SYSTEM:
            {
            } break;

            case OC_COMPONENT_TYPE_LIGHT:
            {
            } break;

            case OC_COMPONENT_TYPE_DYNAMICS_BODY:
            {
            } break;

            default:
            {
                // Could be a custom component. Just skip over it.
            } break;
        }
    }
}

void ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) return;

    // If you trigger this assert it means you've mismatched your world and object.
    ocAssert(pObject->pWorld == pWorld);

    // Objects in each sub-world need to be removed, however we do _not_ remove any components.
    for (uint16_t iComponent = 0; iComponent < pObject->componentCount; ++iComponent) {
        switch (pObject->ppComponents[iComponent]->type)
        {
            case OC_COMPONENT_TYPE_MESH:
            {
            } break;

            case OC_COMPONENT_TYPE_PARTICLE_SYSTEM:
            {
            } break;

            case OC_COMPONENT_TYPE_LIGHT:
            {
            } break;

            case OC_COMPONENT_TYPE_DYNAMICS_BODY:
            {
            } break;

            default:
            {
                // Could be a custom component. Just skip over it.
            } break;
        }
    }
}