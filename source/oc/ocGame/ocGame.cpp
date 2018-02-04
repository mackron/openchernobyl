// Copyright (C) 2018 David Reid. See included LICENSE file.

#include "ocGame.hpp"

ocGame g_Game;

OC_PRIVATE ocResult ocGame_RecreateWindowRT(ocVSyncMode vsyncMode)
{
    ocGraphicsWorldDeleteRT(&g_Game.world.graphicsWorld, g_Game.pWindowRT);
    g_Game.pWindowRT = NULL;

    ocGraphicsDeleteSwapchain(&g_Game.engine.graphics, g_Game.pSwapchain);
    g_Game.pSwapchain = NULL;


    ocResult result = ocGraphicsCreateSwapchain(&g_Game.engine.graphics, &g_Game.window, vsyncMode, &g_Game.pSwapchain);
    if (result != OC_RESULT_SUCCESS) {
        ocErrorf(&g_Game.engine, "Error (%d) recreating the swapchain.", result);
        return result;
    }

    result = ocGraphicsWorldCreateRTFromSwapchain(&g_Game.world.graphicsWorld, g_Game.pSwapchain, &g_Game.pWindowRT);
    if (result != OC_RESULT_SUCCESS) {
        ocErrorf(&g_Game.engine, "Error (%d) recreating the render target.", result);
        return result;
    }

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE void ocGame_OnStep(ocEngineContext* pEngine)
{
    (void)pEngine;

    double dt = ocTimerTick(&g_Game.timer);

    //printf("Step: %f\n", dt);

    // Simple animation test.
    //ocGraphicsWorldSetObjectPosition(&g_Game.world.graphicsWorld, g_Game.pMeshObject, g_Game.pMeshObject->_position + glm::vec4(0.1f * dt, 0, 0, 0));
    //ocGraphicsWorldSetObjectScale(&g_Game.world.graphicsWorld, g_Game.pMeshObject, glm::vec3(0.5f, 0.5f, 0.5f));
    //g_Game.pWindowRT->view = glm::translate(g_Game.pWindowRT->view, glm::vec3(0.1f * dt, 0, 0));


    ocWorldStep(&g_Game.world, dt);
    ocWorldDraw(&g_Game.world);

    // Present last.
    ocGraphicsPresent(&g_Game.engine.graphics, g_Game.pSwapchain);
}

OC_PRIVATE void ocGame_OnWindowEvent(ocEngineContext* pEngine, ocWindowEvent e)
{
    (void)pEngine;

    // Don't care about any events before initialization is complete.
    if ((g_Game.flags & OC_GAME_FLAG_IS_INITIALIZED) == 0) {
        return;
    }

    switch (e.type)
    {
        case OC_WINDOW_EVENT_SIZE:
        {
            // The render target and swapchain for this window needs to be deleted and recreated.
            ocGame_RecreateWindowRT(g_Game.pSwapchain->vsyncMode);
        } break;


        case OC_WINDOW_EVENT_MOUSE_MOVE:
        {
        } break;

        case OC_WINDOW_EVENT_MOUSE_BUTTON_DOWN:
        {
        } break;

        case OC_WINDOW_EVENT_MOUSE_BUTTON_UP:
        {
        } break;


        case OC_WINDOW_EVENT_KEY_DOWN:
        {
            // TESTING

            // V-Sync switching.
            {
                ocBool32 vsyncChanged = false;
                ocVSyncMode vsyncMode = ocVSyncMode_Enabled;
                if (e.data.key_down.key == 'Q') {
                    vsyncChanged = true;
                    vsyncMode = ocVSyncMode_Disabled;
                } else if (e.data.key_down.key == 'W') {
                    vsyncChanged = true;
                    vsyncMode = ocVSyncMode_Enabled;
                } else if (e.data.key_down.key == 'E') {
                    vsyncChanged = true;
                    vsyncMode = ocVSyncMode_Adaptive;
                }

                if (vsyncChanged) {
                    ocGame_RecreateWindowRT(vsyncMode);
                }
            }

            // MSAA
            {
                unsigned int msaa = 0;
                if (e.data.key_down.key == '1') {
                    msaa = 1;
                } else if (e.data.key_down.key == '2') {
                    msaa = 2;
                } else if (e.data.key_down.key == '3') {
                    msaa = 4;
                } else if (e.data.key_down.key == '4') {
                    msaa = 8;
                }

                if (msaa != 0) {
                    // Changing MSAA requires us to completely uninitialize the graphics system and re-create it. Thanks to Vulkan for that one.
                    // TODO: Come up with a solution for this one. May need to make it so that it can only be changed from the main menu.
                }
            }


        } break;

        case OC_WINDOW_EVENT_KEY_UP:
        {
        } break;

        default: break;
    }
}

int ocInitAndRun(int argc, char** argv)
{
    // Clear the global data to 0 before doing anything.
    ocZeroObject(&g_Game);

    // Engine.
    ocResult result = ocEngineInit(argc, argv, ocGame_OnStep, ocGame_OnWindowEvent, &g_Game, &g_Game.engine);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }


    // Window.
    if (!ocWindowInit(&g_Game.engine, 640, 480, &g_Game.window)) {
        goto done;
    }

    // Swapchain.
    result = ocGraphicsCreateSwapchain(&g_Game.engine.graphics, &g_Game.window, ocVSyncMode_Enabled, &g_Game.pSwapchain);
    if (result != OC_RESULT_SUCCESS) {
        goto done;
    }

    ocWindowShow(&g_Game.window);


    // World.
    result = ocWorldInit(&g_Game.engine, &g_Game.world);
    if (result != OC_RESULT_SUCCESS) {
        goto done;
    }

    // Render target for the main window.
    result = ocWorldCreateRTFromSwapchain(&g_Game.world, g_Game.pSwapchain, &g_Game.pWindowRT);
    if (result != OC_RESULT_SUCCESS) {
        goto done;
    }

    // TESTING (Image)
    {
#if 0
        uint32_t sizeX = 2;
        uint32_t sizeY = 2;
        uint8_t data[] = {
            0x00, 0x00, 0x00, 0xFF,    0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF,    0x00, 0x00, 0x00, 0xFF,

            0xFF, 0x00, 0x00, 0xFF
            //0x80, 0x80, 0x80, 0xFF
        };

        ocGraphicsMipmapDesc mipmaps[2];
        mipmaps[0].sizeX = sizeX;
        mipmaps[0].sizeY = sizeY;
        mipmaps[0].dataSize = mipmaps[0].sizeX*mipmaps[0].sizeY*4;
        mipmaps[1].sizeX = 1;
        mipmaps[1].sizeY = 1;
        mipmaps[1].dataSize = mipmaps[1].sizeX*mipmaps[1].sizeY*4;

        ocGraphicsImageDesc desc;
        desc.format = ocGraphicsImageFormat_R8G8B8A8;
        desc.usage = OC_GRAPHICS_IMAGE_USAGE_SHADER_INPUT;
        desc.mipLevels = ocCountOf(mipmaps);
        desc.pMipmaps = mipmaps;
        desc.imageDataSize = sizeof(data);
        desc.pImageData = data;
        result = ocGraphicsCreateImage(&g_Game.engine.graphics, &desc, &g_Game.pImage);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        g_Game.world.graphicsWorld.pCurrentImage = g_Game.pImage;
#endif

#if 0
        ocImageData data;
        result = ocResourceLoaderLoadImage(&g_Game.engine.resourceLoader, "happy_smiley_face.png", &data);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        ocGraphicsImageDesc desc;
        desc.usage = OC_GRAPHICS_IMAGE_USAGE_SHADER_INPUT;
        desc.format = data.format;
        desc.mipLevels = data.mipmapCount;
        desc.pMipmaps = data.pMipmaps;
        desc.imageDataSize = data.imageDataSize;
        desc.pImageData = data.pImageData;
        result = ocGraphicsCreateImage(&g_Game.engine.graphics, &desc, &g_Game.pImage);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        g_Game.world.graphicsWorld.pCurrentImage = g_Game.pImage;
#endif

        result = ocResourceLibraryLoad(&g_Game.engine.resourceLibrary, "happy_smiley_face.png", &g_Game.pImageResource);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        g_Game.world.graphicsWorld.pCurrentImage = g_Game.pImageResource->image.pGraphicsImage;
    }



    // TESTING (Mesh)
    {
        float pVertices[] = {
             0,     0.5f, 0,   0.5f, 1,   0, 0, 1,
            -0.5f, -0.5f, 0,   0, 0,   0, 0, 1,
             0.5f, -0.5f, 0,   1, 0,   0, 0, 1
        };

        uint32_t pIndices[] = {
            0, 1, 2
        };

        ocGraphicsMeshDesc desc;
        desc.primitiveType = ocGraphicsPrimitiveType_Triangle;
        desc.vertexFormat = ocGraphicsVertexFormat_P3T2N3;
        desc.vertexCount = 3;
        desc.pVertices = pVertices;
        desc.indexFormat = ocGraphicsIndexFormat_UInt32;
        desc.indexCount = sizeof(pIndices) / sizeof(pIndices[0]);
        desc.pIndices = pIndices;
        
        result = ocGraphicsCreateMesh(&g_Game.engine.graphics, &desc, &g_Game.pMesh);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

#if 0
        result = ocGraphicsWorldCreateMeshObject(&g_Game.world.graphicsWorld, g_Game.pMesh, &g_Game.pMeshObject);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        ocGraphicsWorldSetObjectPosition(&g_Game.world.graphicsWorld, g_Game.pMeshObject, glm::vec3(0.2f, 0, 0));
#endif


        // Object Test.
        result = ocWorldObjectInit(&g_Game.world, &g_Game.object);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        ocComponent* pComponent = ocWorldObjectAddComponent(&g_Game.object, OC_COMPONENT_TYPE_MESH);
        if (pComponent == NULL) {
            return -1;
        }

        ocComponentMeshSetMesh(OC_MESH_COMPONENT(pComponent), g_Game.pMesh);
        ocWorldInsertObject(&g_Game.world, &g_Game.object);
        ocWorldObjectSetPosition(&g_Game.object, glm::vec3(0.2f, 0, 0));
        ocWorldObjectSetScale(&g_Game.object, glm::vec3(0.5f, 0.5f, 0.5f));
    }


    // TESTING (Scene Resources)
    {
        result = ocResourceLibraryLoad(&g_Game.engine.resourceLibrary, "cube.obj", &g_Game.pSceneResource);
        if (result != OC_RESULT_SUCCESS) {
            return result;
        }

        for (size_t iObject = 0; iObject < g_Game.pSceneResource->scene.objectCount; ++iObject) {
            ocSceneObject* pSceneObject = &g_Game.pSceneResource->scene.pObjects[iObject];

            ocWorldObject object;
            result = ocWorldObjectInit(&g_Game.world, &object);
            if (result != OC_RESULT_SUCCESS) {
                return result;
            }

            // Name.
            ocWorldObjectSetName(&object, (const char*)g_Game.pSceneResource->scene.pPayload + pSceneObject->nameOffset);

            // Transform.
            ocWorldObjectSetTransform(&object,
                glm::vec3(pSceneObject->relativePositionX, pSceneObject->relativePositionY, pSceneObject->relativePositionZ),
                glm::quat(pSceneObject->relativeRotationW, pSceneObject->relativeRotationX, pSceneObject->relativeRotationY, pSceneObject->relativeRotationZ),
                glm::vec3(pSceneObject->relativeScaleX,    pSceneObject->relativeScaleY,    pSceneObject->relativeScaleZ));

            // Components.
            ocSceneObjectComponent* pSceneObjectComponents = (ocSceneObjectComponent*)(g_Game.pSceneResource->scene.pPayload + pSceneObject->componentsOffset);
            for (ocUInt32 iComponent = 0; iComponent < pSceneObject->componentCount; ++iComponent) {
                ocSceneObjectComponent* pSceneObjectComponent = &pSceneObjectComponents[iComponent];

                ocUInt64 componentDataOffset = pSceneObjectComponent->dataOffset;
                ocUInt8* pComponentData = g_Game.pSceneResource->scene.pPayload + componentDataOffset;
                
                switch (pSceneObjectComponent->type) {
                    case OC_COMPONENT_TYPE_SCENE:
                    {
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
                            result = ocGraphicsCreateMesh(&g_Game.engine.graphics, &desc, &pMesh);
                            if (result != OC_RESULT_SUCCESS) {
                                return result;
                            }

                            ocComponent* pComponent = ocWorldObjectAddComponent(&object, OC_COMPONENT_TYPE_MESH);
                            if (pComponent == NULL) {
                                return OC_RESULT_UNKNOWN_ERROR;
                            }

                            result = ocComponentMeshSetMesh(OC_MESH_COMPONENT(pComponent), pMesh);
                            if (result != OC_RESULT_SUCCESS) {
                                return result;
                            }
                        }
                    } break;
                }
            }

            ocWorldInsertObject(&g_Game.world, &object);
        }
    }



    // Timer for the main simulation.
    ocTimerInit(&g_Game.timer);

    // Mark the game as initialized. This is mainly used for ensuring we don't try handling window events prematurely.
    g_Game.flags |= OC_GAME_FLAG_IS_INITIALIZED;

    // The main loop will call ocStep(), and will return when the application has terminated. The return value is the result code.
    result = ocMainLoop(&g_Game.engine);

done:
    ocGraphicsWorldDeleteRT(&g_Game.world.graphicsWorld, g_Game.pWindowRT);
    ocGraphicsDeleteSwapchain(&g_Game.engine.graphics, g_Game.pSwapchain);
    ocWorldUninit(&g_Game.world);
    ocWindowUninit(&g_Game.window);
    ocEngineUninit(&g_Game.engine);
    return result;
}


// Make sure the engine is implemented. Do this last.
#include "../ocEngine/ocEngine.cpp"
