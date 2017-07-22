// Copyright (C) 2017 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
//
// GraphicsContextBase
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocGraphicsInitBase(ocEngineContext* pEngine, ocGraphicsContextBase* pGraphics)
{
    if (pGraphics == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pGraphics);

    if (pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    pGraphics->pEngine = pEngine;

    return OC_RESULT_SUCCESS;
}

void ocGraphicsUninitBase(ocGraphicsContextBase* pGraphics)
{
    if (pGraphics == NULL) return;
}



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsSwapchainBase
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocGraphicsSwapchainBaseInit(ocGraphicsContext* pGraphics, ocWindow* pWindow, ocVSyncMode vsyncMode, ocGraphicsSwapchainBase* pSwapchain)
{
    if (pSwapchain == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pSwapchain);

    if (pGraphics == NULL || pWindow == NULL) return OC_RESULT_INVALID_ARGS;

    pSwapchain->pGraphics = pGraphics;
    pSwapchain->pWindow   = pWindow;
    pSwapchain->vsyncMode = vsyncMode;

    return OC_RESULT_SUCCESS;
}

void ocGraphicsSwapchainBaseUninit(ocGraphicsSwapchainBase* pSwapchain)
{
    if (pSwapchain == NULL) return;
}



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsWorldBase
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocGraphicsWorldInitBase(ocGraphicsContext* pGraphics, ocGraphicsWorldBase* pWorld)
{
    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pWorld);

    if (pGraphics == NULL) return OC_RESULT_INVALID_ARGS;

    pWorld->pGraphics = pGraphics;

    return OC_RESULT_SUCCESS;
}

void ocGraphicsWorldUninitBase(ocGraphicsWorldBase* pWorld)
{
    if (pWorld == NULL) return;
}


ocResult ocGraphicsObjectBaseInit(ocGraphicsWorld* pWorld, ocGraphicsObjectType type, ocGraphicsObjectBase* pObject)
{
    if (pObject == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pObject);

    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;

    pObject->pWorld = pWorld;
    pObject->type = type;

    return OC_RESULT_SUCCESS;
}

void ocGraphicsObjectBaseUninit(ocGraphicsObjectBase* pObject)
{
    if (pObject == NULL) return;
}



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsRTBase
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocGraphicsRTInitBase(ocGraphicsWorld* pWorld, ocGraphicsRTBase* pRT)
{
    if (pRT == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pRT);

    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;

    pRT->pWorld = pWorld;

    return OC_RESULT_SUCCESS;
}

void ocGraphicsRTUninitBase(ocGraphicsRTBase* pRT)
{
    if (pRT == NULL) return;
}



///////////////////////////////////////////////////////////////////////////////
//
// Backends
//
///////////////////////////////////////////////////////////////////////////////
#ifdef OC_USE_OPENGL
#include "OpenGL/ocGraphics_OpenGL.cpp"
#endif

#ifdef OC_USE_VULKAN
#include "Vulkan/ocGraphics_Vulkan.cpp"
#endif