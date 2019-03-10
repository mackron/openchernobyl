// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocLoggerInit(ocEngineContext* pEngine, ocLogger* pLogger)
{
    if (pLogger == NULL) return OC_INVALID_ARGS;
    ocZeroObject(pLogger);

    if (pEngine == NULL) return OC_INVALID_ARGS;

    pLogger->pEngine = pEngine;

    if (ocCmdLineIsSet(pEngine->argc, pEngine->argv, "--silent")) {
        pLogger->noTerminalOutput = OC_TRUE;
    }

    char logFilePath[OC_MAX_PATH];
    ocGetLogFolderPath(&pEngine->fs, logFilePath, sizeof(logFilePath));
    ocPathAppend(logFilePath, sizeof(logFilePath), logFilePath, OC_LOG_FILE_NAME);   // In-place append.

    ocResult result = ocFileOpen(&pEngine->fs, logFilePath, OC_WRITE | OC_CREATE_DIRS, &pLogger->file);
    if (result != OC_SUCCESS) {
        // We failed to create the log file, but it's not a critical error. Just mark it as such so we don't try to use the file.
        pLogger->noFileOutput = OC_TRUE;
    }

    return OC_SUCCESS;
}

void ocLoggerUninit(ocLogger* pLogger)
{
    if (pLogger == NULL) return;

    if (ocLoggerIsFileOutputEnabled(pLogger)) {
        ocFileClose(&pLogger->file);
    }
}


ocBool32 ocLoggerIsTerminalOutputEnabled(ocLogger* pLogger)
{
    if (pLogger == NULL) return false;
    return pLogger->noTerminalOutput == OC_FALSE;
}

ocBool32 ocLoggerIsFileOutputEnabled(ocLogger* pLogger)
{
    if (pLogger == NULL) return false;
    return pLogger->noFileOutput == OC_FALSE;
}


void ocLoggerPrint(ocLogger* pLogger, const char* message)
{
    if (pLogger == NULL || message == NULL) return;

    // Log file.
    if (ocLoggerIsFileOutputEnabled(pLogger)) {
        char dateTime[64];
        ocDateTimeShort(ocNow(), dateTime, sizeof(dateTime));

        ocFileWriteString(&pLogger->file, "[");
        ocFileWriteString(&pLogger->file, dateTime);
        ocFileWriteString(&pLogger->file, "]");
        ocFileWriteLine  (&pLogger->file, message);
        ocFileFlush(&pLogger->file);
    }

    // Terminal.
    if (ocLoggerIsTerminalOutputEnabled(pLogger)) {
        printf("%s\n", message);
    }
}