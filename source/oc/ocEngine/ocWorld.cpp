// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocWorldInit(ocEngineContext* pEngine, ocWorld* pWorld)
{
    if (pWorld == NULL) {
        return OC_INVALID_ARGS;
    }

    ocZeroObject(pWorld);

    if (pEngine == NULL) {
        return OC_INVALID_ARGS;
    }

    pWorld->pEngine = pEngine;


    // Graphics.
    ocResult result = ocGraphicsWorldInit(&pEngine->graphics, &pWorld->graphicsWorld);
    if (result != OC_SUCCESS) {
        return result;
    }

    // Audio.
    result = ocAudioWorldInit(&pEngine->audio, &pWorld->audioWorld);
    if (result != OC_SUCCESS) {
        ocGraphicsWorldUninit(&pWorld->graphicsWorld);
        return result;
    }

    // Physics.
    result = ocDynamicsWorldInit(&pWorld->dynamicsWorld);
    if (result != OC_SUCCESS) {
        ocAudioWorldUninit(&pWorld->audioWorld);
        ocGraphicsWorldUninit(&pWorld->graphicsWorld);
        return result;
    }

    return OC_SUCCESS;
}

void ocWorldUninit(ocWorld* pWorld)
{
    if (pWorld == NULL) {
        return;
    }

    ocDynamicsWorldUninit(&pWorld->dynamicsWorld);
    ocAudioWorldUninit(&pWorld->audioWorld);
    ocGraphicsWorldUninit(&pWorld->graphicsWorld);
}


ocResult ocWorldCreateRTFromSwapchain(ocWorld* pWorld, ocGraphicsSwapchain* pSwapchain, ocGraphicsRT** ppRT)
{
    if (pWorld == NULL) {
        return OC_INVALID_ARGS;
    }

    return ocGraphicsWorldCreateRTFromSwapchain(&pWorld->graphicsWorld, pSwapchain, ppRT);
}

void ocWorldDeleteRT(ocWorld* pWorld, ocGraphicsRT* pRT)
{
    if (pWorld == NULL) {
        return;
    }

    ocGraphicsWorldDeleteRT(&pWorld->graphicsWorld, pRT);
}


void ocWorldStep(ocWorld* pWorld, double dt)
{
    if (pWorld == NULL) {
        return;
    }
    
    ocDynamicsWorldStep(&pWorld->dynamicsWorld, dt);
    ocGraphicsWorldStep(&pWorld->graphicsWorld, dt);
}

void ocWorldDraw(ocWorld* pWorld)
{
    if (pWorld == NULL) {
        return;
    }

    ocGraphicsWorldDraw(&pWorld->graphicsWorld);
}


void ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    // If you trigger this assert it means you've mismatched your world and object.
    ocAssert(pObject->pWorld == pWorld);

    // We need to go over each component and create the relevant objects for the the sub-worlds. The connections between the
    // object and the sub-worlds are achieved with an attribute in each component.
    for (uint16_t iComponent = 0; iComponent < pObject->componentCount; ++iComponent) {
        switch (pObject->ppComponents[iComponent]->type)
        {
            case OC_COMPONENT_TYPE_MESH:
            {
                ocMeshComponent* pMeshComponent = OC_MESH_COMPONENT(pObject->ppComponents[iComponent]);
                ocAssert(pMeshComponent->pMeshObject == NULL);  // <-- You've done something wrong if the mesh object is not null at this point.
                ocGraphicsWorldCreateMeshObject(&pWorld->graphicsWorld, pMeshComponent->pMesh, &pMeshComponent->pMeshObject);
                ocGraphicsWorldSetObjectTransform(&pWorld->graphicsWorld, pMeshComponent->pMeshObject, pObject->position, pObject->rotation, pObject->scale);
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

    pObject->isInWorld = OC_TRUE;
}

void ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    // If you trigger this assert it means you've mismatched your world and object.
    ocAssert(pObject->pWorld == pWorld);

    // Objects in each sub-world need to be removed, however we do _not_ remove any components.
    for (uint16_t iComponent = 0; iComponent < pObject->componentCount; ++iComponent) {
        switch (pObject->ppComponents[iComponent]->type)
        {
            case OC_COMPONENT_TYPE_MESH:
            {
                ocMeshComponent* pMeshComponent = OC_MESH_COMPONENT(pObject->ppComponents[iComponent]);
                if (pMeshComponent->pMeshObject != NULL) {
                    ocGraphicsWorldDeleteObject(&pWorld->graphicsWorld, pMeshComponent->pMeshObject);
                    pMeshComponent->pMeshObject = NULL;
                }
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

    pObject->isInWorld = OC_FALSE;
}

void ocWorldSetObjectPosition(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &position)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    return ocWorldSetObjectTransform(pWorld, pObject, position, pObject->rotation, glm::vec3(pObject->scale));
}

void ocWorldSetObjectRotation(ocWorld* pWorld, ocWorldObject* pObject, const glm::quat &rotation)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    return ocWorldSetObjectTransform(pWorld, pObject, pObject->position, rotation, glm::vec3(pObject->scale));
}

void ocWorldSetObjectScale(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &scale)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    return ocWorldSetObjectTransform(pWorld, pObject, pObject->position, pObject->rotation, scale);
}

void ocWorldSetObjectTransform(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    pObject->position = glm::vec4(position, 0);
    pObject->rotation = rotation;
    pObject->scale    = glm::vec4(scale, 0);

    // If the object is already in the world we'll need to update the backends.
    if (ocWorldObjectIsInWorld(pObject)) {
        for (uint16_t iComponent = 0; iComponent < pObject->componentCount; ++iComponent) {
            switch (pObject->ppComponents[iComponent]->type)
            {
                case OC_COMPONENT_TYPE_MESH:
                {
                    ocMeshComponent* pMeshComponent = OC_MESH_COMPONENT(pObject->ppComponents[iComponent]);
                    ocGraphicsWorldSetObjectTransform(&pWorld->graphicsWorld, pMeshComponent->pMeshObject, position, rotation, scale);
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
            }
        }
    }
}