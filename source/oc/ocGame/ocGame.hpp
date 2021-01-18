// Copyright (C) 2018 David Reid. See included LICENSE file.

#ifndef OC_GAME_HPP
#define OC_GAME_HPP

#include "../ocEngine/ocEngine.hpp"

struct ocGame
{
    ocEngineContext engine;
    ocWindow window;
    ocTimer timer;
    ocWorld world;
    ocGraphicsSwapchain* pSwapchain;
    ocGraphicsRT* pWindowRT;
    ocBool32 isInitialized : 1;

    // TESTING
    ocCamera camera;
    ocResource* pImageResource;
    ocResource* pSceneResource;
    ocGraphicsImage* pImage;
    ocGraphicsMesh* pMesh;
    ocGraphicsObject* pMeshObject;
    ocWorldObject object;
};
extern ocGame g_Game;

int ocInitAndRun(int argc, char** argv);

#endif