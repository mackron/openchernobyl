// Copyright (C) 2016 David Reid. See included LICENSE file.

#include "ocGraphics_OpenGL_Autogen.cpp"

///////////////////////////////////////////////////////////////////////////////
//
// GraphicsContext
//
///////////////////////////////////////////////////////////////////////////////

GLint ocToOpenGLImageInternalFormat(ocGraphicsImageFormat format)
{
    switch (format)
    {
        case ocGraphicsImageFormat_R8G8B8:          return GL_RGB;
        case ocGraphicsImageFormat_R8G8B8_SRGB:     return GL_SRGB;
        case ocGraphicsImageFormat_R8G8B8A8:        return GL_RGBA;
        case ocGraphicsImageFormat_R8G8B8A8_SRGB:   return GL_SRGB_ALPHA;
        default: return GL_RGBA;
    }
}

GLint ocToOpenGLImageFormat(ocGraphicsImageFormat format)
{
    switch (format)
    {
        case ocGraphicsImageFormat_R8G8B8:          return GL_RGB;
        case ocGraphicsImageFormat_R8G8B8_SRGB:     return GL_RGB;
        case ocGraphicsImageFormat_R8G8B8A8:        return GL_RGBA;
        case ocGraphicsImageFormat_R8G8B8A8_SRGB:   return GL_RGBA;
        default: return GL_RGBA;
    }
}

GLenum ocToOpenGLImageFormatType(ocGraphicsImageFormat format)
{
    // Currently, all image formats are unsigned bytes.
    (void)format;
    return GL_UNSIGNED_BYTE;
}

OC_PRIVATE void ocGraphicsSetCurrentWindow(ocGraphicsContext* pGraphics, ocWindow* pWindow)
{
    if (pGraphics == NULL || pWindow == NULL) return;
    if (pGraphics->pCurrentWindow == pWindow) return;

#ifdef OC_WIN32
    pGraphics->gl.MakeCurrent(pWindow->hDC, pGraphics->gl.hRC);
#endif

#ifdef OC_X11
    pGraphics->gl.MakeCurrent(pGraphics->gl.pDisplay, pWindow->windowX11, pGraphics->gl.rc);
#endif

    pGraphics->pCurrentWindow = pWindow;
}


ocResult ocGraphicsInit(ocGraphicsContext* pGraphics, ocEngineContext* pEngine, uint32_t desiredMSAASamples)
{
    ocResult result = ocGraphicsInitBase(pGraphics, pEngine);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    if (!drglInit(&pGraphics->gl)) {
        return OC_RESULT_FAILED_TO_INIT_GRAPHICS;
    }

    drgl &gl = pGraphics->gl;

    // Feature support.
    if (drglIsExtensionSupported(&gl, "GL_ARB_texture_multisample")) {
        pGraphics->supportFlags |= OC_GRAPHICS_SUPPORT_FLAG_MSAA;
        pGraphics->minMSAA = 1;

        GLint maxMSAA = 1;
        gl.GetIntegerv(GL_MAX_SAMPLES, &maxMSAA);
        pGraphics->maxMSAA = maxMSAA;

        pGraphics->msaaSamples = ocClamp(desiredMSAASamples, pGraphics->minMSAA, pGraphics->maxMSAA);

        // TODO: Only enable this when it's needed.
        if (pGraphics->msaaSamples > 1) {
            pGraphics->gl.Enable(GL_MULTISAMPLE);
        }
    }

    if (drglIsExtensionSupported(&gl, "WGL_EXT_swap_control_tear") || drglIsExtensionSupported(&gl, "GLX_EXT_swap_control_tear")) {
        pGraphics->supportFlags |= OC_GRAPHICS_SUPPORT_FLAG_ADAPTIVE_VSYNC;
    }




    pGraphics->pCurrentWindow = NULL;

    return OC_RESULT_SUCCESS;
}

void ocGraphicsUninit(ocGraphicsContext* pGraphics)
{
    if (pGraphics == NULL) return;

    drglUninit(&pGraphics->gl);
    ocGraphicsUninitBase(pGraphics);
}


ocResult ocGraphicsCreateSwapchain(ocGraphicsContext* pGraphics, ocWindow* pWindow, ocVSyncMode vsyncMode, ocGraphicsSwapchain** ppSwapchain)
{
    if (ppSwapchain == NULL) return OC_RESULT_INVALID_ARGS;
    *ppSwapchain = NULL;

    ocGraphicsSwapchain* pSwapchain = ocCallocObject(ocGraphicsSwapchain);
    if (pSwapchain == NULL) {
        return OC_RESULT_OUT_OF_MEMORY;
    }

    ocResult result = ocGraphicsSwapchainBaseInit(pSwapchain, pGraphics, pWindow, vsyncMode);
    if (result != OC_RESULT_SUCCESS) {
        ocFree(pSwapchain);
        return result;
    }


#ifdef OC_WIN32
    if (!SetPixelFormat(pWindow->hDC, pGraphics->gl.pixelFormat, &pGraphics->gl.pfd)) {
        ocFree(pSwapchain);
        return OC_RESULT_FAILED_TO_INIT_GRAPHICS;
    }
#endif


    // V-Sync.
    if (pGraphics->gl.SwapIntervalEXT != NULL) {
        int interval = 0;
        if (vsyncMode == ocVSyncMode_Enabled) {
            interval = 1;
        } else if (vsyncMode == ocVSyncMode_Adaptive && ocIsBitSet(pGraphics->supportFlags, OC_GRAPHICS_SUPPORT_FLAG_ADAPTIVE_VSYNC)) {
            interval = -1;
        }

        ocWindow* pPrevCurrentWindow = pGraphics->pCurrentWindow;
        ocGraphicsSetCurrentWindow(pGraphics, pWindow);
        {
#ifdef OC_WIN32
            pGraphics->gl.SwapIntervalEXT(interval);
#endif
#ifdef OC_X11
            pGraphics->gl.SwapIntervalEXT(pGraphics->gl.pDisplay, pWindow->windowX11, interval);
#endif
        }
        ocGraphicsSetCurrentWindow(pGraphics, pPrevCurrentWindow);
    }


    *ppSwapchain = pSwapchain;
    return OC_RESULT_SUCCESS;
}

void ocGraphicsDeleteSwapchain(ocGraphicsContext* pGraphics, ocGraphicsSwapchain* pSwapchain)
{
    if (pGraphics == NULL || pSwapchain == NULL) return;

    ocGraphicsSwapchainBaseUninit(pSwapchain);
}

void ocGraphicsGetSwapchainSize(ocGraphicsContext* pGraphics, ocGraphicsSwapchain* pSwapchain, uint32_t* pSizeX, uint32_t* pSizeY)
{
    if (pSizeX != NULL) *pSizeX = 0;
    if (pSizeY != NULL) *pSizeY = 0;
    if (pGraphics == NULL || pSwapchain == NULL) return;

    unsigned int sizeX;
    unsigned int sizeY;
    ocWindowGetSize(pSwapchain->pWindow, &sizeX, &sizeY);

    if (pSizeX != NULL) *pSizeX = (unsigned int)sizeX;
    if (pSizeY != NULL) *pSizeY = (unsigned int)sizeY;
}

void ocGraphicsPresent(ocGraphicsContext* pGraphics, ocGraphicsSwapchain* pSwapchain)
{
    if (pGraphics == NULL || pSwapchain == NULL) return;

#ifdef OC_WIN32
    SwapBuffers(pSwapchain->pWindow->hDC);
#endif

#ifdef OC_X11
    pGraphics->gl.SwapBuffers(pGraphics->gl.pDisplay, pSwapchain->pWindow->windowX11);
#endif
}


ocResult ocGraphicsCreateImage(ocGraphicsContext* pGraphics, ocGraphicsImageDesc* pDesc, ocGraphicsImage** ppImage)
{
    if (ppImage == NULL) return OC_RESULT_INVALID_ARGS;
    *ppImage = NULL;

    if (pGraphics == NULL || pDesc == NULL) return OC_RESULT_INVALID_ARGS;

    ocGraphicsImage* pImage = ocCallocObject(ocGraphicsImage);
    if (pImage == NULL) {
        return OC_RESULT_OUT_OF_MEMORY;
    }

    drgl &gl = pGraphics->gl;

    GLint internalFormat = ocToOpenGLImageInternalFormat(pDesc->format);
    GLint format = ocToOpenGLImageFormat(pDesc->format);
    GLenum type = ocToOpenGLImageFormatType(pDesc->format);

    GLenum error = gl.GetError();

    gl.GenTextures(1, &pImage->objectGL);
    gl.BindTexture(GL_TEXTURE_2D, pImage->objectGL);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*GL_NEAREST_MIPMAP_NEAREST*/);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    size_t runningOffset = 0;
    for (uint32_t iMipmap = 0; iMipmap < pDesc->mipLevels; ++iMipmap) {
        gl.TexImage2D(GL_TEXTURE_2D, iMipmap, internalFormat, (GLsizei)pDesc->pMipmaps[iMipmap].sizeX, (GLsizei)pDesc->pMipmaps[iMipmap].sizeY, 0, format, type, ocOffsetPtr(pDesc->pImageData, runningOffset));
        runningOffset += pDesc->pMipmaps[iMipmap].dataSize;
    }

    error = gl.GetError();

    *ppImage = pImage;
    return OC_RESULT_SUCCESS;
}

void ocGraphicsDeleteImage(ocGraphicsContext* pGraphics, ocGraphicsImage* pImage)
{
    if (pGraphics == NULL || pImage == NULL) return;

    pGraphics->gl.DeleteTextures(1, &pImage->objectGL);
    ocFree(pImage);
}


ocResult ocGraphicsCreateMesh(ocGraphicsContext* pGraphics, ocGraphicsMeshDesc* pDesc, ocGraphicsMesh** ppMesh)
{
    if (pGraphics == NULL || pDesc == NULL || ppMesh == NULL) return OC_RESULT_INVALID_ARGS;

    ocGraphicsMesh* pMesh = ocCallocObject(ocGraphicsMesh);
    if (pMesh == NULL) {
        return OC_RESULT_OUT_OF_MEMORY;
    }

    pMesh->format = pDesc->format;
    pMesh->vertexCount = pDesc->vertexCount;
    pMesh->indexCount = pDesc->indexCount;

    size_t vertexBufferSize = pMesh->vertexCount * ocGetVertexSizeFromFormat(pMesh->format);
    size_t indexBufferSize = pMesh->indexCount * sizeof(uint32_t);

    drgl &gl = pGraphics->gl;

    gl.GenBuffers(1, &pMesh->vbo);
    gl.BindBuffer(GL_ARRAY_BUFFER, pMesh->vbo);
    gl.BufferData(GL_ARRAY_BUFFER, vertexBufferSize, pDesc->pVertices, GL_STATIC_DRAW);

    gl.GenBuffers(1, &pMesh->ibo);
    gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->ibo);
    gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, pDesc->pIndices, GL_STATIC_DRAW);

    *ppMesh = pMesh;
    return OC_RESULT_SUCCESS;
}

void ocGraphicsDeleteMesh(ocGraphicsContext* pGraphics, ocGraphicsMesh* pMesh)
{
    if (pGraphics == NULL || pMesh == NULL) return;

    drgl &gl = pGraphics->gl;

    gl.DeleteBuffers(1, &pMesh->ibo);
    gl.DeleteBuffers(1, &pMesh->vbo);

    ocFree(pMesh);
}



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsWorld
//
///////////////////////////////////////////////////////////////////////////////

ocResult ocGraphicsWorldInit(ocGraphicsWorld* pWorld, ocGraphicsContext* pGraphics)
{
    ocResult result = ocGraphicsWorldInitBase(pWorld, pGraphics);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pWorld->pObjects = new std::vector<ocGraphicsObject*>();


    drgl &gl = pWorld->pGraphics->gl;

    // Shaders. Should probably make this part of the context...
    char* errorStr;
    pWorld->testProgramGL = drglCreateSimpleProgramFromStrings(&gl, g_VertexShader_Test, g_FragmentShader_Test, &errorStr);
    if (pWorld->testProgramGL == 0) {
        ocErrorf(pGraphics->pEngine, "%s\n", errorStr);
        drglFree(errorStr);
        return OC_RESULT_SHADER_ERROR;
    }
    drglFree(errorStr);


    return OC_RESULT_SUCCESS;
}

void ocGraphicsWorldUninit(ocGraphicsWorld* pWorld)
{
    if (pWorld == NULL) return;

    delete pWorld->pObjects;
    ocGraphicsWorldUninitBase(pWorld);
}


void ocGraphicsWorldDraw(ocGraphicsWorld* pWorld)
{
    if (pWorld == NULL) return;

    for (uint16_t iRT = 0; iRT < pWorld->renderTargetCount; ++iRT) {
        ocGraphicsWorldDrawRT(pWorld, pWorld->pRenderTargets[iRT]);
    }
}

void ocGraphicsWorldDrawRT(ocGraphicsWorld* pWorld, ocGraphicsRT* pRT)
{
    if (pWorld == NULL || pRT == NULL) return;
    drgl &gl = pWorld->pGraphics->gl;

    gl.BindFramebufferEXT(GL_FRAMEBUFFER, pRT->framebuffer.objectGL);
    gl.Viewport(0, 0, pRT->sizeX, pRT->sizeY);

    gl.UseProgram(pWorld->testProgramGL);

    // Camera.
    gl.UniformMatrix4fv(gl.GetUniformLocation(pWorld->testProgramGL, "Projection"), 1, GL_FALSE, glm::value_ptr(pRT->projection));
    gl.UniformMatrix4fv(gl.GetUniformLocation(pWorld->testProgramGL, "View"),       1, GL_FALSE, glm::value_ptr(pRT->view));

    gl.ClearColor(0, 0, 1, 1);
    gl.ClearDepth(1);
    gl.ClearStencil(0);
    gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    gl.BindTexture(GL_TEXTURE_2D, pWorld->pCurrentImage->objectGL);

    for (size_t iObject = 0; iObject < pWorld->pObjects->size(); ++iObject) {
        ocGraphicsObject* pObject = pWorld->pObjects->at(iObject);
        if (pObject->type == ocGraphicsObjectType_Mesh) {
            ocGraphicsMesh* pMesh = pObject->data.mesh.pResource;   // <-- For ease of use.

            gl.UniformMatrix4fv(gl.GetUniformLocation(pWorld->testProgramGL, "Model"), 1, GL_FALSE, glm::value_ptr(pObject->_transform));
            gl.Uniform1i(gl.GetUniformLocation(pWorld->testProgramGL, "Texture"), 0);

            gl.EnableClientState(GL_VERTEX_ARRAY);
            gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
            gl.EnableClientState(GL_NORMAL_ARRAY);

            gl.BindBuffer(GL_ARRAY_BUFFER, pMesh->vbo);
            gl.VertexPointer(3, GL_FLOAT, ocGetVertexSizeFromFormat(pMesh->format), 0);
            gl.TexCoordPointer(2, GL_FLOAT, ocGetVertexSizeFromFormat(pMesh->format), (const GLvoid*)(sizeof(float)*(3)));
            gl.NormalPointer(GL_FLOAT, ocGetVertexSizeFromFormat(pMesh->format), (const GLvoid*)(sizeof(float)*(3+2)));

            gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->ibo);
            gl.DrawElements(GL_TRIANGLES, pMesh->indexCount, GL_UNSIGNED_INT, 0);
        }
    }


    // The final composition is slightly different depending on whether or not we are outputting to a window or an image.
    if (pRT->pSwapchain != NULL) {
        ocGraphicsSetCurrentWindow(pWorld->pGraphics, pRT->pSwapchain->pWindow);
        gl.BindFramebufferEXT(GL_READ_FRAMEBUFFER, pRT->framebuffer.objectGL);
        gl.BindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
        gl.BlitFramebufferEXT(0, 0, pRT->sizeX, pRT->sizeY, 0, 0, pRT->sizeX, pRT->sizeY, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    } else {

    }
}


void ocGraphicsWorldStep(ocGraphicsWorld* pWorld, double dt)
{
    if (pWorld == NULL || dt == 0) return;

    // TODO: Step animations and particle effects.
}



OC_PRIVATE ocResult ocGraphicsWorldAddRT(ocGraphicsWorld* pWorld, ocGraphicsRT* pRT)
{
    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;
    if (pWorld->renderTargetCount == OC_MAX_RENDER_TARGETS) return OC_RESULT_TOO_MANY_RENDER_TARGETS;

    pWorld->pRenderTargets[pWorld->renderTargetCount++] = pRT;

    return OC_RESULT_SUCCESS;
}

OC_PRIVATE void ocGraphicsWorldRemoveRTByIndex(ocGraphicsWorld* pWorld, uint16_t index)
{
    ocAssert(pWorld != NULL);
    if (pWorld->renderTargetCount == 0) return;

    for (uint16_t i = index; i < pWorld->renderTargetCount-1; ++i) {
        pWorld->pRenderTargets[i] = pWorld->pRenderTargets[i+1];
    }

    pWorld->renderTargetCount -= 1;
}

OC_PRIVATE void ocGraphicsWorldRemoveRT(ocGraphicsWorld* pWorld, ocGraphicsRT* pRT)
{
    if (pWorld == NULL || pRT == NULL) return;

    for (uint16_t iRT = 0; iRT < pWorld->renderTargetCount; ++iRT) {
        if (pWorld->pRenderTargets[iRT] == pRT) {
            ocGraphicsWorldRemoveRTByIndex(pWorld, iRT);
            break;
        }
    }
}

OC_PRIVATE const char* ocOpenGLFramebufferStatusToString(GLenum status)
{
    switch (status)
    {
        case GL_FRAMEBUFFER_COMPLETE:                      return "GL_FRAMEBUFFER_COMPLETE";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
        case GL_FRAMEBUFFER_UNSUPPORTED:                   return "GL_FRAMEBUFFER_UNSUPPORTED";
        default: return "Unknown";
    }
}

OC_PRIVATE ocResult ocGraphicsWorldAllocAndInitRT(ocGraphicsWorld* pWorld, ocGraphicsRT** ppRT, uint32_t sizeX, uint32_t sizeY)
{
    if (ppRT == NULL) return OC_RESULT_INVALID_ARGS;
    *ppRT = NULL;   // Safety.

    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;

    ocGraphicsRT* pRT = (ocGraphicsRT*)calloc(1, sizeof(*pRT));
    if (pRT == NULL) return OC_RESULT_OUT_OF_MEMORY;

    ocResult result = ocGraphicsRTInitBase(pRT, pWorld);
    if (result != OC_RESULT_SUCCESS) {
        free(pRT);
        return result;
    }

    pRT->sizeX = (GLsizei)sizeX;
    pRT->sizeY = (GLsizei)sizeY;
    pRT->projection = glm::mat4();
    pRT->view = glm::mat4();

    // Render targets need a framebuffer object.
    drgl &gl = pWorld->pGraphics->gl;

    // Color texture.
    gl.GenTextures(1, &pRT->framebuffer.colorTextureGL);
    if (pWorld->pGraphics->msaaSamples == 1) {
        gl.BindTexture(GL_TEXTURE_2D, pRT->framebuffer.colorTextureGL);
        gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)sizeX, (GLsizei)sizeY, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    } else {
        gl.BindTexture(GL_TEXTURE_2D_MULTISAMPLE, pRT->framebuffer.colorTextureGL);
        gl.TexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gl.TexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
        gl.TexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl.TexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl.TexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, pWorld->pGraphics->msaaSamples, GL_RGBA8, (GLsizei)sizeX, (GLsizei)sizeY, GL_FALSE);      // <-- TODO: Check for an extension.
    }

    // Depth/stencil renderbuffer.
    gl.GenRenderbuffersEXT(1, &pRT->framebuffer.depthStencilRenderbufferGL);
    gl.BindRenderbufferEXT(GL_RENDERBUFFER, pRT->framebuffer.depthStencilRenderbufferGL);
    if (pWorld->pGraphics->msaaSamples == 1) {
        gl.RenderbufferStorageEXT(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)sizeX, (GLsizei)sizeY);
    } else {
        gl.RenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, pWorld->pGraphics->msaaSamples, GL_DEPTH24_STENCIL8, pRT->sizeX, pRT->sizeY);
    }


    // Framebuffer object.
    gl.GenFramebuffersEXT(1, &pRT->framebuffer.objectGL);
    gl.BindFramebufferEXT(GL_FRAMEBUFFER, pRT->framebuffer.objectGL);
    if (pWorld->pGraphics->msaaSamples == 1) {
        gl.FramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pRT->framebuffer.colorTextureGL, 0);
        gl.FramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pRT->framebuffer.depthStencilRenderbufferGL);
    } else {
        gl.FramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, pRT->framebuffer.colorTextureGL, 0);
        gl.FramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_RENDERBUFFER, pRT->framebuffer.depthStencilRenderbufferGL);
    }

    GLenum status = gl.CheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        ocErrorf(pWorld->pGraphics->pEngine, "Invalid framebuffer status: %s", ocOpenGLFramebufferStatusToString(status));
        return OC_RESULT_INVALID_FRAMEBUFFER;
    }


    *ppRT = pRT;
    return OC_RESULT_SUCCESS;
}

OC_PRIVATE void ocGraphicsWorldUninitRT(ocGraphicsWorld* pWorld, ocGraphicsRT* pRT)
{
    ocAssert(pWorld != NULL);
    ocAssert(pRT != NULL);

    drgl &gl = pWorld->pGraphics->gl;

    gl.DeleteFramebuffersEXT(1, &pRT->framebuffer.objectGL);
    gl.DeleteRenderbuffersEXT(1, &pRT->framebuffer.depthStencilRenderbufferGL);
    gl.DeleteTextures(1, &pRT->framebuffer.colorTextureGL);
}

ocResult ocGraphicsWorldCreateRTFromSwapchain(ocGraphicsWorld* pWorld, ocGraphicsSwapchain* pSwapchain, ocGraphicsRT** ppRT)
{
    uint32_t sizeX;
    uint32_t sizeY;
    ocGraphicsGetSwapchainSize(pWorld->pGraphics, pSwapchain, &sizeX, &sizeY);

    ocResult result = ocGraphicsWorldAllocAndInitRT(pWorld, ppRT, sizeX, sizeY);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    ocGraphicsRT* pRT = *ppRT;
    pRT->pSwapchain = pSwapchain;
    pRT->pImage = NULL;

    result = ocGraphicsWorldAddRT(pWorld, pRT);
    if (result != OC_RESULT_SUCCESS) {
        ocGraphicsRTUninitBase(pRT);
        return result;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocGraphicsWorldCreateRTFromImage(ocGraphicsWorld* pWorld, ocGraphicsImage* pImage, ocGraphicsRT** ppRT)
{
    ocResult result = ocGraphicsWorldAllocAndInitRT(pWorld, ppRT, pImage->sizeX, pImage->sizeY);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    ocGraphicsRT* pRT = *ppRT;
    pRT->pSwapchain = NULL;
    pRT->pImage = pImage;

    result = ocGraphicsWorldAddRT(pWorld, pRT);
    if (result != OC_RESULT_SUCCESS) {
        ocGraphicsRTUninitBase(pRT);
        return result;
    }

    return OC_RESULT_SUCCESS;
}

void ocGraphicsWorldDeleteRT(ocGraphicsWorld* pWorld, ocGraphicsRT* pRT)
{
    if (pWorld == NULL || pRT == NULL) return;

    ocGraphicsWorldUninitRT(pWorld, pRT);

    ocGraphicsWorldRemoveRT(pWorld, pRT);
    ocGraphicsRTUninitBase(pRT);
}


OC_PRIVATE ocResult ocGraphicsObjectInit(ocGraphicsObject* pObject, ocGraphicsWorld* pWorld, ocGraphicsObjectType type)
{
    ocResult result = ocGraphicsObjectBaseInit(pObject, pWorld, type);
    if (result != OC_RESULT_SUCCESS) {
        ocFree(pObject);
        return result;
    }

    pObject->_position  = glm::vec4(0, 0, 0, 0);
    pObject->_rotation  = glm::quat(1, 0, 0, 0);
    pObject->_scale     = glm::vec4(1, 1, 1, 1);
    pObject->_transform = glm::mat4();

    return OC_RESULT_SUCCESS;
}

ocResult ocGraphicsWorldCreateMeshObject(ocGraphicsWorld* pWorld, ocGraphicsMesh* pMesh, ocGraphicsObject** ppObjectOut)
{
    if (pWorld == NULL || pMesh == NULL || ppObjectOut == NULL) return OC_RESULT_INVALID_ARGS;

    ocGraphicsObject* pObject = ocMallocObject(ocGraphicsObject);
    if (pObject == NULL) {
        return OC_RESULT_OUT_OF_MEMORY;
    }

    ocResult result = ocGraphicsObjectInit(pObject, pWorld, ocGraphicsObjectType_Mesh);
    if (result != OC_RESULT_SUCCESS) {
        ocFree(pObject);
        return result;
    }

    pObject->data.mesh.pResource = pMesh;

    // Add the object to the world. Should this be done explicitly at a higher level for consistency with ocWorld?
    pWorld->pObjects->push_back(pObject);

    *ppObjectOut = pObject;
    return OC_RESULT_SUCCESS;
}

void ocGraphicsWorldDeleteObject(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject)
{
    if (pWorld == NULL || pObject == NULL) return;

    pWorld->pObjects->erase(std::remove(pWorld->pObjects->begin(), pWorld->pObjects->end(), pObject), pWorld->pObjects->end());

    ocFree(pObject);
}


void ocGraphicsWorldSetObjectPosition(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::vec3 &position)
{
    ocGraphicsWorldSetObjectTransform(pWorld, pObject, position, pObject->_rotation, glm::vec3(pObject->_scale));
}

void ocGraphicsWorldSetObjectRotation(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::quat &rotation)
{
    ocGraphicsWorldSetObjectTransform(pWorld, pObject, glm::vec3(pObject->_position), rotation, glm::vec3(pObject->_scale));
}

void ocGraphicsWorldSetObjectScale(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::vec3 &scale)
{
    ocGraphicsWorldSetObjectTransform(pWorld, pObject, glm::vec3(pObject->_position), pObject->_rotation, scale);
}

void ocGraphicsWorldSetObjectTransform(ocGraphicsWorld* pWorld, ocGraphicsObject* pObject, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
{
    if (pWorld == NULL || pObject == NULL) return;
    pObject->_position  = glm::vec4(position, 0);
    pObject->_rotation  = rotation;
    pObject->_scale     = glm::vec4(scale, 1);
    pObject->_transform = ocMakeMat4(pObject->_position, pObject->_rotation, pObject->_scale);
}

