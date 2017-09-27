// Copyright (C) 2017 David Reid. See included LICENSE file.

#define OC_ENGINE_FLAG_PORTABLE     (1 << 0)

typedef void (* ocStepProc)       (ocEngineContext* pEngine);
typedef void (* ocWindowEventProc)(ocEngineContext* pEngine, ocWindowEvent e);

struct ocEngineContext
{
    int argc;
    char** argv;
    ocStepProc onStep;
    ocWindowEventProc onWindowEvent;
    void* pUserData;

    ocFileSystem fs;
    ocLogger logger;
    ocGraphicsContext graphics;
    ocAudioContext audio;
    ocComponentAllocator componentAllocator;
    ocResourceLoader resourceLoader;
    ocResourceLibrary resourceLibrary;
    ocUInt32 threadCount;
    ocUInt32 flags;
};

// ocEngineInit
ocResult ocEngineInit(int argc, char** argv, ocStepProc onStep, ocWindowEventProc onWindowEvent, void* pUserData, ocEngineContext* pEngine);

// ocEngineUninit
void ocEngineUninit(ocEngineContext* pEngine);


// Steps the game.
//
// You should not normally need to call this directly - it will be called by the platform layer in the main loop.
//
// This is where the onStep callback that was passed into ocEngineInit() is called.
void ocStep(ocEngineContext* pEngine);

// Handles a window event. 
//
// You should not normally need to call this directly - it will be called by the platform layer in the main loop.
void ocHandleWindowEvent(ocEngineContext* pEngine, ocWindowEvent e);


// Whether or not we are running the portable version of the game.
ocBool32 ocIsPortable(ocEngineContext* pEngine);


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


