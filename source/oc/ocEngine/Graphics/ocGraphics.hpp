// Copyright (C) 2016 David Reid. See included LICENSE file.

// Hardware support flags.
#define OC_GRAPHICS_SUPPORT_FLAG_MSAA               (1 << 0)
#define OC_GRAPHICS_SUPPORT_FLAG_ADAPTIVE_VSYNC     (1 << 1)

enum ocVSyncMode
{
    ocVSyncMode_Disabled,   // No vsync.
    ocVSyncMode_Enabled,    // Standard vsync.
    ocVSyncMode_Adaptive    // Tear if frame rate is lower than monitor refresh rate, otherwise vsync.
};

enum ocGraphicsObjectType
{
    ocGraphicsObjectType_Mesh,
    ocGraphicsObjectType_ParticleSystem,
    ocGraphicsObjectType_AmbientLight,
    ocGraphicsObjectType_DirectionalLight,
    ocGraphicsObjectType_PointLight,
    ocGraphicsObjectType_SpotLight,
};

enum ocGraphicsVertexFormat
{
    ocGraphicsVertexFormat_P3T2N3,          // <-- Normal mapping disabled
    ocGraphicsVertexFormat_P3T2N3T3B3       // <-- Normal mapping enabled
};

OC_INLINE size_t ocGetVertexSizeFromFormat(ocGraphicsVertexFormat format)
{
    switch (format)
    {
        case ocGraphicsVertexFormat_P3T2N3:     return sizeof(float) * (3+2+3);
        case ocGraphicsVertexFormat_P3T2N3T3B3: return sizeof(float) * (3+2+3+3+3);
    }

    return 0;
}

struct ocGraphicsMeshDesc
{
    ocGraphicsVertexFormat format;
    float* pVertices;
    uint32_t vertexCount;
    uint32_t* pIndices;
    uint32_t indexCount;

    // TODO: Material information.
};


#define OC_GRAPHICS_IMAGE_USAGE_SHADER_INPUT    (1 << 0)
#define OC_GRAPHICS_IMAGE_USAGE_RENDER_TARGET   (1 << 1)

struct ocGraphicsImageDesc
{
    ocImageFormat format;
    uint32_t usage;
    uint32_t mipLevels;
    ocMipmapInfo* pMipmaps;
    size_t imageDataSize;
    const void* pImageData;
};


///////////////////////////////////////////////////////////////////////////////
//
// GraphicsContextBase
//
// This is implemented in ocGraphics.cpp.
//
///////////////////////////////////////////////////////////////////////////////
struct ocGraphicsContextBase
{
    ocEngineContext* pEngine;
    uint32_t supportFlags;
    uint32_t minMSAA;
    uint32_t maxMSAA;
};

//
ocResult ocGraphicsInitBase(ocGraphicsContextBase* pGraphics, ocEngineContext* pEngine);

//
void ocGraphicsUninitBase(ocGraphicsContextBase* pGraphics);


///////////////////////////////////////////////////////////////////////////////
//
// GraphicsSwapchainBase
//
///////////////////////////////////////////////////////////////////////////////

struct ocGraphicsSwapchainBase
{
    ocGraphicsContext* pGraphics;
    ocWindow* pWindow;
    ocVSyncMode vsyncMode;
};

//
ocResult ocGraphicsSwapchainBaseInit(ocGraphicsSwapchainBase* pSwapchain, ocGraphicsContext* pGraphics, ocWindow* pWindow, ocVSyncMode vsyncMode);

//
void ocGraphicsSwapchainBaseUninit(ocGraphicsSwapchainBase* pSwapchain);


///////////////////////////////////////////////////////////////////////////////
//
// GraphicsWorldBase
//
///////////////////////////////////////////////////////////////////////////////
struct ocGraphicsWorldBase
{
    ocGraphicsContext* pGraphics;
};

//
ocResult ocGraphicsWorldInitBase(ocGraphicsWorldBase* pWorld, ocGraphicsContext* pGraphics);

//
void ocGraphicsWorldUninitBase(ocGraphicsWorldBase* pWorld);


struct ocGraphicsObjectBase
{
    ocGraphicsWorld* pWorld;
    ocGraphicsObjectType type;
};

//
ocResult ocGraphicsObjectBaseInit(ocGraphicsObjectBase* pObject, ocGraphicsWorld* pWorld, ocGraphicsObjectType type);

//
void ocGraphicsObjectBaseUninit(ocGraphicsObjectBase* pObject);


///////////////////////////////////////////////////////////////////////////////
//
// GraphicsRTBase
//
///////////////////////////////////////////////////////////////////////////////
struct ocGraphicsRTBase
{
    ocGraphicsWorld* pWorld;
};

//
ocResult ocGraphicsRTInitBase(ocGraphicsRTBase* pRT, ocGraphicsWorld* pWorld);

//
void ocGraphicsRTUninitBase(ocGraphicsRTBase* pRT);




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Everything below needs to be implemented for each rendering backend. Note that only one backend can be compiled into the executable at a time since they
// all implement the same APIs. Backends are _not_ implemented as virtual classes.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ocGraphicsContext;
struct ocGraphicsSwapchain;
struct ocGraphicsWorld;
struct ocGraphicsObject;
struct ocGraphicsImage;
struct ocGraphicsMesh;
struct ocGraphicsFB;
struct ocGraphicsRT;




///////////////////////////////////////////////////////////////////////////////
//
// GraphicsContext
//
// This is implemented in either ocGraphics_OpenGL.cpp or ocGraphics_Vulkan.cpp.
//
///////////////////////////////////////////////////////////////////////////////

// ocGraphicsInit
ocResult ocGraphicsInit(ocGraphicsContext* pGraphics, ocEngineContext* pEngine, uint32_t desiredMSAASamples);

// ocGraphicsUninit
void ocGraphicsUninit(ocGraphicsContext* pGraphics);


// Creates a swapchain tied to the given window.
ocResult ocGraphicsCreateSwapchain(ocGraphicsContext* pGraphics, ocWindow* pWindow, ocVSyncMode vsyncMode, ocGraphicsSwapchain** ppSwapchain);

// Deletes a swapchain.
void ocGraphicsDeleteSwapchain(ocGraphicsContext* pGraphics, ocGraphicsSwapchain* pSwapchain);

// Retrieves the dimensions of the given swapchain.
void ocGraphicsGetSwapchainSize(ocGraphicsContext* pGraphics, ocGraphicsSwapchain* pSwapchain, uint32_t* pSizeX, uint32_t* pSizeY);

// ocGraphicsPresent
void ocGraphicsPresent(ocGraphicsContext* pGraphics, ocGraphicsSwapchain* pSwapchain);


// Creates an image for use in materials or render targets.
ocResult ocGraphicsCreateImage(ocGraphicsContext* pGraphics, ocGraphicsImageDesc* pDesc, ocGraphicsImage** ppImage);

// Deletes an image.
void ocGraphicsDeleteImage(ocGraphicsContext* pGraphics, ocGraphicsImage* pImage);


// ocGraphicsCreateMesh
ocResult ocGraphicsCreateMesh(ocGraphicsContext* pGraphics, ocGraphicsMeshDesc* pDesc, ocGraphicsMesh** ppMesh);

// ocGraphicsDeleteMesh
void ocGraphicsDeleteMesh(ocGraphicsContext* pGraphics, ocGraphicsMesh* pMesh);



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsWorld
//
///////////////////////////////////////////////////////////////////////////////

// ocGraphicsWorldInit()
ocResult ocGraphicsWorldInit(ocGraphicsWorld* pWorld, ocGraphicsContext* pGraphics);

// ocGraphicsWorldUninit()
void ocGraphicsWorldUninit(ocGraphicsWorld* pWorld);

// Monolithic draw function for the world. This will iterate over each render target and draw them, but will
// will not present them. Use ocGraphicsWorldDrawRT() to draw a specific render target.
void ocGraphicsWorldDraw(ocGraphicsWorld* pWorld);

// Draws an individual render target.
void ocGraphicsWorldDrawRT(ocGraphicsWorld* pWorld, ocGraphicsRT* pRT);

// ocGraphicsWorldStep()
void ocGraphicsWorldStep(ocGraphicsWorld* pWorld, double dt);


// ocGraphicsWorldCreateRTFromWindow
ocResult ocGraphicsWorldCreateRTFromSwapchain(ocGraphicsWorld* pWorld, ocGraphicsSwapchain* pSwapchain, ocGraphicsRT** ppRT);

// ocGraphicsWorldCreateRTFromImage
ocResult ocGraphicsWorldCreateRTFromImage(ocGraphicsWorld* pWorld, ocGraphicsImage* pImage, ocGraphicsRT** ppRT);

// Deleting a render target does not delete the underlying window or image.
void ocGraphicsWorldDeleteRT(ocGraphicsWorld* pWorld, ocGraphicsRT* pRT);


// Creates a mesh object.
ocResult ocGraphicsWorldCreateMeshObject(ocGraphicsWorld* pWorld, ocGraphicsMesh* pMesh, ocGraphicsObject** ppObjectOut);

// Deletes the given graphics object.
void ocGraphicsWorldDeleteObject(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject);


// Sets the position of the given object.
//
// Use ocGraphicsWorldSetObjectTransform() if you want to change the position, rotation and/or scale at the same time.
void ocGraphicsWorldSetObjectPosition(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::vec3 &position);

// Sets the rotation of the given object.
//
// Use ocGraphicsWorldSetObjectTransform() if you want to change the position, rotation and/or scale at the same time.
void ocGraphicsWorldSetObjectRotation(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::quat &rotation);

// Sets the scale of the given object.
//
// Use ocGraphicsWorldSetObjectTransform() if you want to change the position, rotation and/or scale at the same time.
void ocGraphicsWorldSetObjectScale(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::vec3 &scale);

// Sets the position, rotation and scale of an object as a single operation.
void ocGraphicsWorldSetObjectTransform(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale);


///////////////////////////////////////////////////////////////////////////////
//
// Backends
//
///////////////////////////////////////////////////////////////////////////////
#ifdef OC_USE_OPENGL
#include "OpenGL/ocGraphics_OpenGL.hpp"
#endif

#ifdef OC_USE_VULKAN
#include "Vulkan/ocGraphics_Vulkan.hpp"
#endif