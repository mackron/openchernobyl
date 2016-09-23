// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocLoggerInit(ocLogger* pLogger, ocEngineContext* pEngine)
{
    if (pLogger == NULL || pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pLogger);
    pLogger->pEngine = pEngine;

    if (ocCmdLineIsSet(pEngine->argc, pEngine->argv, "--silent")) {
        pLogger->flags |= OC_LOGGER_FLAG_NO_TERMINAL_OUTPUT;
    }

    char logFilePath[OC_MAX_PATH];
    ocGetLogFolderPath(&pEngine->fs, logFilePath, sizeof(logFilePath));
    drpath_append(logFilePath, sizeof(logFilePath), OC_LOG_FILE_NAME);

    ocResult result = ocFileOpen(&pEngine->fs, logFilePath, OC_WRITE | OC_CREATE_DIRS, &pLogger->file);
    if (result != OC_RESULT_SUCCESS) {
        // We failed to create the log file, but it's not a critical error. Just mark it as such so we don't try to use the file.
        pEngine->flags |= OC_LOGGER_FLAG_NO_FILE;
    }

    return OC_RESULT_SUCCESS;
}

void ocLoggerUninit(ocLogger* pLogger)
{
    if (pLogger == NULL) return;

    if (ocLoggerIsFileOutputEnabled(pLogger)) {
        ocFileClose(&pLogger->file);
    }
}


bool ocLoggerIsTerminalOutputEnabled(ocLogger* pLogger)
{
    if (pLogger == NULL) return false;
    return (pLogger->flags & OC_LOGGER_FLAG_NO_TERMINAL_OUTPUT) == 0; 
}

bool ocLoggerIsFileOutputEnabled(ocLogger* pLogger)
{
    if (pLogger == NULL) return false;
    return (pLogger->flags & OC_LOGGER_FLAG_NO_FILE) == 0;
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