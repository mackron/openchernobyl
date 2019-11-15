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


OC_PRIVATE ocBool32 ocWorld_DoesSceneResourceHaveSingleParent(ocResource* pSceneResource)
{
    ocAssert(pSceneResource != NULL);

    ocUInt32 counter = 0;
    for (size_t iObject = 0; iObject < pSceneResource->scene.objectCount; ++iObject) {
        ocSceneObject* pSceneObject = &pSceneResource->scene.pObjects[iObject];
        if (pSceneObject->parentIndex == OC_SCENE_OBJECT_NONE) {
            counter += 1;
            if (counter > 1) {
                return OC_FALSE;
            }
        }
    }

    return OC_TRUE;
}

OC_PRIVATE ocResult ocWorldCreateObjectFromResource_Scene(ocWorld* pWorld, ocResource* pSceneResource, ocResourceLibrary* pResourceLibrary, ocWorldObject** ppObject)
{
    ocAssert(pWorld != NULL);
    ocAssert(pSceneResource != NULL);
    ocAssert(ppObject != NULL);

    ocBool32 isExplicitRootRequired = OC_FALSE;
    if (pSceneResource->scene.objectCount == 0) {
        isExplicitRootRequired = OC_TRUE;
    } else {
        // If there are multiple root elements in the scene resource we'll need to wrap it in a root element.
        isExplicitRootRequired = !ocWorld_DoesSceneResourceHaveSingleParent(pSceneResource);
    }

    ocUInt32 objectCount = pSceneResource->scene.objectCount;
    if (isExplicitRootRequired) {
        objectCount += 1;   // An extra object for the root.
    }

    ocWorldObject* pObjects = (ocWorldObject*)ocMalloc(sizeof(*pObjects) * objectCount);
    if (pObjects == NULL) {
        return OC_OUT_OF_MEMORY;
    }

    // Do an initial pass to initialize each object.
    for (ocUInt32 iObject = 0; iObject < objectCount; ++iObject) {
        ocWorldObjectInit(pWorld, &pObjects[iObject]);
    }

    // Mark the first object as the one that has the memory allocation.
    pObjects[0].isMemoryOwnedByWorld = OC_TRUE;


    ocWorldObject* pFirstObject = pObjects;
    if (isExplicitRootRequired) {
        pFirstObject += 1;
    }

    // Now we can go through each object and set everything up. This is actually done in two passes. The first pass sets the majority
    // of everything up _except_ for the sibling order. The second pass gets siblings into the correct order.
    for (size_t iObject = 0; iObject < pSceneResource->scene.objectCount; ++iObject) {
        ocSceneObject* pSceneObject = &pSceneResource->scene.pObjects[iObject];
        ocWorldObject* pWorldObject = &pFirstObject[iObject];

        // Name.
        ocWorldObjectSetName(pWorldObject, (const char*)pSceneResource->scene.pPayload + pSceneObject->nameOffset);


        // Hierarchy.
        {
            ocWorldObject* pParentWorldObject = NULL;
            if (pSceneObject->parentIndex != OC_SCENE_OBJECT_NONE) {
                pParentWorldObject = &pFirstObject[pSceneObject->parentIndex];
            } else {
                if (isExplicitRootRequired) {
                    pParentWorldObject = &pObjects[0];
                }
            }

            if (pParentWorldObject != NULL) {
                ocWorldObjectAppendChild(pParentWorldObject, pWorldObject);
            } else {
                // In this case it's the root item. No hierarchy operations need to be done here.
            }
        }


        // Transform.
        ocWorldObjectSetAbsoluteTransform(pWorldObject,
            glm::vec3(pSceneObject->absolutePositionX, pSceneObject->absolutePositionY, pSceneObject->absolutePositionZ),
            glm::quat(pSceneObject->absoluteRotationW, pSceneObject->absoluteRotationX, pSceneObject->absoluteRotationY, pSceneObject->absoluteRotationZ),
            glm::vec3(pSceneObject->absoluteScaleX,    pSceneObject->absoluteScaleY,    pSceneObject->absoluteScaleZ));


        // Components.
        ocSceneObjectComponent* pSceneObjectComponents = (ocSceneObjectComponent*)(pSceneResource->scene.pPayload + pSceneObject->componentsOffset);
        for (ocUInt32 iComponent = 0; iComponent < pSceneObject->componentCount; ++iComponent) {
            ocSceneObjectComponent* pSceneObjectComponent = &pSceneObjectComponents[iComponent];

            ocUInt64 componentDataOffset = pSceneObjectComponent->dataOffset;
            ocUInt8* pComponentData = pSceneResource->scene.pPayload + componentDataOffset;
                
            switch (pSceneObjectComponent->type) {
                case OC_COMPONENT_TYPE_SCENE:
                {
                    // This is a sub-resource that needs to be loaded in.
                } break;

                case OC_COMPONENT_TYPE_MESH:
                {
                    ocUInt32 groupCount       = *(ocUInt32*)(pComponentData + 0);
                    //ocUInt32 padding          = *(ocUInt32*)(pComponentData + 4);
                    //ocUInt64 vertexDataSize   = *(ocUInt64*)(pComponentData + 8);
                    ocUInt64 vertexDataOffset = *(ocUInt64*)(pComponentData + 16);
                    //ocUInt64 indexDataSize    = *(ocUInt64*)(pComponentData + 24);
                    ocUInt64 indexDataOffset  = *(ocUInt64*)(pComponentData + 32);

                    ocUInt8* pVertexData = pComponentData + vertexDataOffset;
                    ocUInt8* pIndexData  = pComponentData + indexDataOffset;

                    ocOCDSceneBuilderMeshGroup* pGroups = (ocOCDSceneBuilderMeshGroup*)(pComponentData + 40);
                    for (ocUInt32 iGroup = 0; iGroup < groupCount; ++iGroup) {
                        ocOCDSceneBuilderMeshGroup* pGroup = &pGroups[iGroup];

                        ocGraphicsMeshDesc desc;
                        desc.primitiveType = (ocGraphicsPrimitiveType)pGroup->primitiveType;
                        desc.vertexFormat  = (ocGraphicsVertexFormat)pGroup->vertexFormat;
                        desc.vertexCount   = pGroup->vertexCount;
                        desc.pVertices     = pVertexData + pGroup->vertexDataOffset;
                        desc.indexFormat   = (ocGraphicsIndexFormat)pGroup->indexFormat;
                        desc.indexCount    = pGroup->indexCount;
                        desc.pIndices      = pIndexData + pGroup->indexDataOffset;

                        ocGraphicsMesh* pMesh;
                        ocResult result = ocGraphicsCreateMesh(pResourceLibrary->pGraphics, &desc, &pMesh);
                        if (result != OC_SUCCESS) {
                            return result;  // Need better recovery than this.
                        }

                        ocComponent* pComponent = ocWorldObjectAddComponent(pWorldObject, OC_COMPONENT_TYPE_MESH);
                        if (pComponent == NULL) {
                            return OC_ERROR;
                        }

                        result = ocComponentMeshSetMesh(OC_MESH_COMPONENT(pComponent), pMesh);
                        if (result != OC_SUCCESS) {
                            return result;
                        }
                    }
                } break;
            }
        }
    }

    // Get siblings into the correct order.
    for (size_t iObject = 0; iObject < pSceneResource->scene.objectCount; ++iObject) {
        ocSceneObject* pSceneObject = &pSceneResource->scene.pObjects[iObject];
        ocWorldObject* pWorldObject = &pFirstObject[iObject];

        if (pSceneObject->prevSiblingIndex != OC_SCENE_OBJECT_NONE) {
            ocWorldObjectAppendSibling(&pFirstObject[pSceneObject->prevSiblingIndex], pWorldObject);
        } else if (pSceneObject->nextSiblingIndex != OC_SCENE_OBJECT_NONE) {
            ocWorldObjectPrependSibling(&pFirstObject[pSceneObject->nextSiblingIndex], pWorldObject);
        }
    }

    *ppObject = pObjects;
    return OC_SUCCESS;
}

ocResult ocWorldCreateObjectFromResource(ocWorld* pWorld, ocResource* pResource, ocResourceLibrary* pResourceLibrary, ocWorldObject** ppObject)
{
    if (ppObject == NULL) {
        return OC_INVALID_ARGS;
    }

    *ppObject = NULL;   // Safety.

    if (pWorld == NULL || pResource == NULL) {
        return OC_INVALID_ARGS;
    }

    switch (pResource->type)
    {
        case ocResourceType_Scene:
        {
            return ocWorldCreateObjectFromResource_Scene(pWorld, pResource, pResourceLibrary, ppObject);
        } break;

        // Hitting the default case means we can't do anything with this resource type.
        default: return OC_INVALID_ARGS;
    }
}

OC_PRIVATE void ocWorldDeleteObjectRecursive(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pObject == NULL) {
        return;
    }

    // Children first.
    for (ocWorldObject* pChild = pObject->pFirstChild; pChild != NULL; pChild = pChild->pNextSibling) {
        ocWorldDeleteObjectRecursive(pWorld, pChild);
    }

    ocWorldObjectUninit(pObject);

    if (pObject->isMemoryOwnedByWorld) {
        ocFree(pObject);
    }
}

void ocWorldDeleteObject(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    // Remove the object from the world first.
    ocWorldRemoveObject(pWorld, pObject);
    
    // Uninitialize and free.
    ocWorldDeleteObjectRecursive(pWorld, pObject);
}


ocResult ocWorldInsertObject(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) {
        return OC_INVALID_ARGS;
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

    // Children last.
    for (ocWorldObject* pChild = pObject->pFirstChild; pChild != NULL; pChild = pChild->pNextSibling) {
        ocWorldInsertObject(pWorld, pChild);
    }

    pObject->isInWorld = OC_TRUE;

    return OC_SUCCESS;
}

ocResult ocWorldRemoveObject(ocWorld* pWorld, ocWorldObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) {
        return OC_INVALID_ARGS;
    }

    // Children first.
    for (ocWorldObject* pChild = pObject->pFirstChild; pChild != NULL; pChild = pChild->pNextSibling) {
        ocWorldRemoveObject(pWorld, pChild);
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

    return OC_SUCCESS;
}


void ocWorldSetObjectAbsoluteTransform(ocWorld* pWorld, ocWorldObject* pObject, const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale)
{
    if (pWorld == NULL || pObject == NULL) {
        return;
    }

    // We update the transforms of the children first, because they need to know the previous transform of the parent
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