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
                ocGraphicsWorldSetObjectTransform(&pWorld->graphicsWorld, pMeshComponent->pMeshObject, pObject->absolutePosition, pObject->absoluteRotation, pObject->absoluteScale);
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


void ocWorldSetObjectAbsoluteTransform(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    // We update the transforms of the children first, because they need to no the previous transform of the parent
    // object so they can be transformed correctly.
    for (ocWorldObject* pChild = pObject->pFirstChild; pChild != NULL; pChild = pChild->pNextSibling) {
        if (pChild->noRelativePosition == 0 || pChild->noRelativeRotation == 0 || pChild->noRelativeScale == 0) {   // <-- Check if anything needs to be updated.
            glm::vec3 childAbsolutePosition = pChild->absolutePosition;
            if (pChild->noRelativePosition == 0) {
                childAbsolutePosition = ocMakeAbsolutePosition(ocWorldObjectGetRelativePosition(pChild), absolutePosition);
            }

            glm::quat childAbsoluteRotation = pChild->absoluteRotation;
            if (pChild->noRelativeRotation == 0) {
                childAbsoluteRotation = ocMakeAbsoluteRotation(ocWorldObjectGetRelativeRotation(pChild), absoluteRotation);
            }

            glm::vec3 childAbsoluteScale = pChild->absoluteScale;
            if (pChild->noRelativeScale == 0) {
                childAbsoluteScale = ocMakeAbsoluteScale(ocWorldObjectGetRelativeScale(pChild), absoluteScale);
            }

            ocWorldSetObjectAbsoluteTransform(pWorld, pChild, childAbsolutePosition, childAbsoluteRotation, childAbsoluteScale);
        }
    }


    pObject->absolutePosition = glm::vec4(absolutePosition, 0);
    pObject->absoluteRotation = absoluteRotation;
    pObject->absoluteScale    = glm::vec4(absoluteScale, 0);

    // If the object is already in the world we'll need to update the backends.
    if (ocWorldObjectIsInWorld(pObject)) {
        for (uint16_t iComponent = 0; iComponent < pObject->componentCount; ++iComponent) {
            switch (pObject->ppComponents[iComponent]->type)
            {
                case OC_COMPONENT_TYPE_MESH:
                {
                    ocMeshComponent* pMeshComponent = OC_MESH_COMPONENT(pObject->ppComponents[iComponent]);
                    ocGraphicsWorldSetObjectTransform(&pWorld->graphicsWorld, pMeshComponent->pMeshObject, absolutePosition, absoluteRotation, absoluteScale);
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