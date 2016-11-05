// Copyright (C) 2016 David Reid. See included LICENSE file.

#ifndef OC_GAME_HPP
#define OC_GAME_HPP

#include "../ocEngine/ocEngine.hpp"

#define OC_GAME_FLAG_IS_INITIALIZED     (1 << 0)

struct ocGame
{
    ocEngineContext engine;
    ocWindow window;
    ocTimer timer;
    ocWorld world;
    ocGraphicsSwapchain* pSwapchain;
    ocGraphicsRT* pWindowRT;
    unsigned int flags;

    // TESTING
    ocResource* pImageResource;
    ocGraphicsImage* pImage;
    ocGraphicsMesh* pMesh;
    ocGraphicsObject* pMeshObject;
};
extern ocGame g_Game;

int ocInitAndRun(int argc, char** argv);

#endif