// Copyright (C) 2017 David Reid. See included LICENSE file.

#define OC_LOGGER_FLAG_NO_TERMINAL_OUTPUT   (1 << 0)
#define OC_LOGGER_FLAG_NO_FILE              (1 << 1)

struct ocLogger
{
    ocEngineContext* pEngine;
    ocFile file;
    uint32_t flags;
};

// Initializes the logging system.
ocResult ocLoggerInit(ocEngineContext* pEngine, ocLogger* pLogger);

// Uninitializes the logging system.
void ocLoggerUninit(ocLogger* pLogger);


// Determines whether or not terminal output is enabled.
dr_bool32 ocLoggerIsTerminalOutputEnabled(ocLogger* pLogger);

// Determines whether or not file output is enabled.
dr_bool32 ocLoggerIsFileOutputEnabled(ocLogger* pLogger);


// Prints a string.
void ocLoggerPrint(ocLogger* pLogger, const char* message);