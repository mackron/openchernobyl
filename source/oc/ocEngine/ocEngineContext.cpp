// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocEngineInit(ocEngineContext* pEngine, int argc, char** argv, void* pUserData)
{
    if (pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    #if defined(OC_USE_OPENGL) && defined(OC_X11)
    uintptr_t props[] = {
        OC_PLATFORM_LAYER_PROP_XDISPLAY, (uintptr_t)pEngine->graphics.gl.pDisplay,
        OC_PLATFORM_LAYER_PROP_XVISUALINFO, (uintptr_t)pEngine->graphics.gl.pFBVisualInfo,
        OC_PLATFORM_LAYER_PROP_XCOLORMAP, pEngine->graphics.gl.colormap,
        0, 0
    };
    #else
    uintptr_t* props = NULL;
    #endif

    ocZeroObject(pEngine);
    pEngine->argc = argc;
    pEngine->argv = argv;
    pEngine->pUserData = pUserData;

    if (ocCmdLineIsSet(argc, argv, "--portable")) {
        pEngine->flags |= OC_ENGINE_FLAG_PORTABLE;
    }


    // File system. This is done early so we can open a log file ASAP.
    ocResult result = ocFileSystemInit(pEngine, &pEngine->fs);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // Logging. Initialize logging early so we can output messages during initialization.
    result = ocLoggerInit(&pEngine->logger, pEngine);
    if (result != OC_RESULT_SUCCESS) {
        goto on_error1;
    }

    // Graphics.
    result = ocGraphicsInit(&pEngine->graphics, pEngine, 4);
    if (result != OC_RESULT_SUCCESS) {
        goto on_error2;
    }

    // Audio.
    result = ocAudioInit(&pEngine->audio, pEngine);
    if (result != OC_RESULT_SUCCESS) {
        goto on_error3;
    }

    // Component allocator.
    result = ocComponentAllocatorInit(&pEngine->componentAllocator, pEngine);
    if (result != OC_RESULT_SUCCESS) {
        goto on_error4;
    }

    // Resource loader.
    result = ocResourceLoaderInit(&pEngine->resourceLoader, &pEngine->fs);
    if (result != OC_RESULT_SUCCESS) {
        goto on_error5;
    }

    // Resource library.
    result = ocResourceLibraryInit(&pEngine->resourceLibrary, &pEngine->resourceLoader, &pEngine->graphics);
    if (result != OC_RESULT_SUCCESS) {
        goto on_error6;
    }


    // The platform layer is initialized a little bit differently depending on the platform. It needs to come after the graphics system is
    // initialized due to the coupling of X11 and OpenGL.
    result = ocPlatformLayerInit(props);
    if (result != OC_RESULT_SUCCESS) {
        goto on_error7;
    }


    return OC_RESULT_SUCCESS;

on_error7: ocResourceLibraryUninit(&pEngine->resourceLibrary);
on_error6: ocResourceLoaderUninit(&pEngine->resourceLoader);
on_error5: ocComponentAllocatorUninit(&pEngine->componentAllocator);
on_error4: ocAudioUninit(&pEngine->audio);
on_error3: ocGraphicsUninit(&pEngine->graphics);
on_error2: ocLoggerUninit(&pEngine->logger);
on_error1: ocFileSystemUninit(&pEngine->fs);
    return result;
}

void ocEngineUninit(ocEngineContext* pEngine)
{
    if (pEngine == NULL) return;

    ocPlatformLayerUninit();
    ocResourceLibraryUninit(&pEngine->resourceLibrary);
    ocResourceLoaderUninit(&pEngine->resourceLoader);
    ocComponentAllocatorUninit(&pEngine->componentAllocator);
    ocAudioUninit(&pEngine->audio);
    ocGraphicsUninit(&pEngine->graphics);
    ocLoggerUninit(&pEngine->logger);
    ocFileSystemUninit(&pEngine->fs);
}


dr_bool32 ocIsPortable(ocEngineContext* pEngine)
{
    if (pEngine == NULL) return false;
    return (pEngine->flags & OC_ENGINE_FLAG_PORTABLE) != 0;
}


void ocLog(ocEngineContext* pEngine, const char* message)
{
    if (pEngine == NULL || message == NULL) {
        return;
    }

    ocLoggerPrint(&pEngine->logger, message);
}

void ocLogf(ocEngineContext* pEngine, const char* format, ...)
{
    if (pEngine == NULL || format == NULL) {
        return;
    }

    va_list args;
    va_start(args, format);
    {
#if 0
        char* msg = ocMakeFormattedStringV(format, args);
        if (msg != NULL) {
            ocLog(pEngine, msg);
            ocFreeString(msg);
        }
#endif

        char msg[4096];
        vsnprintf(msg, sizeof(msg), format, args);

        ocLog(pEngine, msg);
    }
    va_end(args);
}

void ocWarning(ocEngineContext* pEngine, const char* message)
{
    ocLogf(pEngine, "[WARNING] %s", message);
}

void ocWarningf(ocEngineContext* pEngine, const char* format, ...)
{
    if (pEngine == NULL || format == NULL) {
        return;
    }

    va_list args;
    va_start(args, format);
    {
        char msg[4096];
        vsnprintf(msg, sizeof(msg), format, args);

        ocWarning(pEngine, msg);
    }
    va_end(args);
}

void ocError(ocEngineContext* pEngine, const char* message)
{
    ocLogf(pEngine, "[ERROR] %s", message);
}

void ocErrorf(ocEngineContext* pEngine, const char* format, ...)
{
    if (pEngine == NULL || format == NULL) {
        return;
    }

    va_list args;
    va_start(args, format);
    {
        char msg[4096];
        vsnprintf(msg, sizeof(msg), format, args);

        ocError(pEngine, msg);
    }
    va_end(args);
}


ocResult ocRegisterComponent(ocEngineContext* pEngine, ocComponentType type, ocCreateComponentProc onCreate, ocDeleteComponentProc onDelete, void* pUserData)
{
    if (pEngine == NULL) return OC_RESULT_INVALID_ARGS;
    return ocComponentAllocatorRegister(&pEngine->componentAllocator, type, onCreate, onDelete, pUserData);
}

ocComponent* ocCreateComponent(ocEngineContext* pEngine, ocComponentType type, ocWorldObject* pObject)
{
    if (pEngine == NULL) return NULL;
    return ocComponentAllocatorCreateComponent(&pEngine->componentAllocator, type, pObject);
}

void ocDeleteComponent(ocEngineContext* pEngine, ocComponent* pComponent)
{
    if (pEngine == NULL || pComponent == NULL) return;
    return ocComponentAllocatorDeleteComponent(&pEngine->componentAllocator, pComponent);
}
