// Copyright (C) 2016 David Reid. See included LICENSE file.

#define OC_ENGINE_FLAG_PORTABLE     (1 << 0)

struct ocEngineContext
{
    int argc;
    char** argv;

    ocFileSystem fs;
    ocLogger logger;
    ocGraphicsContext graphics;
    ocAudioContext audio;
    ocComponentAllocator componentAllocator;
    ocResourceLoader resourceLoader;
    ocResourceLibrary resourceLibrary;
    uint32_t threadCount;
    uint32_t flags;
};

// ocEngineInit
ocResult ocEngineInit(ocEngineContext* pEngine, int argc, char** argv);

// ocEngineUninit
void ocEngineUninit(ocEngineContext* pEngine);


// Whether or not we are running the portable version of the game.
dr_bool32 ocIsPortable(ocEngineContext* pEngine);


// Posts a log message.
void ocLog(ocEngineContext* pEngine, const char* message);

// Posts a formatted log message.
void ocLogf(ocEngineContext* pEngine, const char* format, ...);

// Posts a warning log message.
void ocWarning(ocEngineContext* pEngine, const char* message);

// Posts a formatted warning log message.
void ocWarningf(ocEngineContext* pEngine, const char* format, ...);

// Posts an error log message.
void ocError(ocEngineContext* pEngine, const char* message);

// Posts a formatted error log message.
void ocErrorf(ocEngineContext* pEngine, const char* format, ...);


// Registers a component.
//
// Components cannot be used until they are registered with this function.
ocResult ocRegisterComponent(ocEngineContext* pEngine, ocComponentType type, ocCreateComponentProc onCreate, ocDeleteComponentProc ocDelete, void* pUserData);

// Creates a component.
ocComponent* ocCreateComponent(ocEngineContext* pEngine, ocComponentType type, ocWorldObject* pObject);

// Deletes a component.
void ocDeleteComponent(ocEngineContext* pEngine, ocComponent* pComponent);


