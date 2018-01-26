// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocLogger
{
    ocEngineContext* pEngine;
    ocFile file;
    ocBool32 noTerminalOutput : 1;
    ocBool32 noFileOutput     : 1;
};

// Initializes the logging system.
ocResult ocLoggerInit(ocEngineContext* pEngine, ocLogger* pLogger);

// Uninitializes the logging system.
void ocLoggerUninit(ocLogger* pLogger);


// Determines whether or not terminal output is enabled.
ocBool32 ocLoggerIsTerminalOutputEnabled(ocLogger* pLogger);

// Determines whether or not file output is enabled.
ocBool32 ocLoggerIsFileOutputEnabled(ocLogger* pLogger);


// Prints a string.
void ocLoggerPrint(ocLogger* pLogger, const char* message);