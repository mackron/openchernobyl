// Copyright (C) 2016 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
//
// GraphicsContextBase
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocGraphicsInitBase(ocGraphicsContextBase* pGraphics, ocEngineContext* pEngine)
{
    if (pGraphics == NULL || pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pGraphics);
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

ocResult ocGraphicsSwapchainBaseInit(ocGraphicsSwapchainBase* pSwapchain, ocGraphicsContext* pGraphics, ocWindow* pWindow, ocVSyncMode vsyncMode)
{
    if (pSwapchain == NULL || pGraphics == NULL || pWindow == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pSwapchain);
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

ocResult ocGraphicsWorldInitBase(ocGraphicsWorldBase* pWorld, ocGraphicsContext* pGraphics)
{
    if (pWorld == NULL || pGraphics == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pWorld);
    pWorld->pGraphics = pGraphics;

    return OC_RESULT_SUCCESS;
}

void ocGraphicsWorldUninitBase(ocGraphicsWorldBase* pWorld)
{
    if (pWorld == NULL) return;
}


ocResult ocGraphicsObjectBaseInit(ocGraphicsObjectBase* pObject, ocGraphicsWorld* pWorld, ocGraphicsObjectType type)
{
    if (pObject == NULL || pWorld == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pObject);
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

ocResult ocGraphicsRTInitBase(ocGraphicsRTBase* pRT, ocGraphicsWorld* pWorld)
{
    if (pRT == NULL || pWorld == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pRT);
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