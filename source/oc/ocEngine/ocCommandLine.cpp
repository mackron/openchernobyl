// Copyright (C) 2016 David Reid. See included LICENSE file.

typedef struct
{
    char* value;

    // Win32 style data.
    char* win32_payload;
    char* valueEnd;
} ocCmdLineIterator;

ocCmdLineIterator ocCmdLineBegin(const char* cmdlineWinMain)
{
    ocCmdLineIterator i;
    i.value         = NULL;
    i.win32_payload = NULL;
    i.valueEnd      = NULL;

    size_t length = strlen(cmdlineWinMain);
    i.win32_payload = (char*)malloc(length + 2);         // +2 for a double null terminator.
    strcpy_s(i.win32_payload, length + 2, cmdlineWinMain);
    i.win32_payload[length + 1] = '\0';
    i.valueEnd = i.win32_payload;

    return i;
}

bool ocCmdLineNext(ocCmdLineIterator* i)
{
    if (i == NULL) {
        return false;
    }

    if (i->value == NULL) {
        i->value    = i->win32_payload;
        i->valueEnd = i->value;
    } else {
        i->value = i->valueEnd + 1;
    }


    // Move to the start of the next argument.
    while (i->value[0] != '\0' && i->value[0] == ' ') {
        i->value += 1;
    }


    // If at this point we are sitting on the null terminator it means we have finished iterating.
    if (i->value[0] == '\0')
    {
        free(i->win32_payload);
        i->win32_payload = NULL;
        i->value = NULL;
        i->valueEnd = NULL;
        return false;
    }


    // Move to the end of the token. If the argument begins with a double quote, we iterate until we find
    // the next unescaped double-quote.
    if (i->value[0] == '\"')
    {
        // Go to the last unescaped double-quote.
        i->value += 1;
        i->valueEnd = i->value + 1;

        while (i->valueEnd[0] != '\0' && i->valueEnd[0] != '\"') {
            if (i->valueEnd[0] == '\\') {
                i->valueEnd += 1;

                if (i->valueEnd[0] == '\0') {
                    break;
                }
            }

            i->valueEnd += 1;
        }
        i->valueEnd[0] = '\0';
    }
    else
    {
        // Go to the next space.
        i->valueEnd = i->value + 1;

        while (i->valueEnd[0] != '\0' && i->valueEnd[0] != ' ') {
            i->valueEnd += 1;
        }
        i->valueEnd[0] = '\0';
    }

    return true;
}


int ocWinMainToArgv(const char* cmdlineWinMain, char*** argvOut)
{
    if (argvOut == NULL) return 0;
    *argvOut = NULL;

    int argc = 0;
    char** argv = NULL;
    size_t cmdlineLen = 0;

    // The command line is parsed in 2 passes. The first pass simple calculates the required sizes of each buffer. The second
    // pass fills those buffers with actual data.

    // First pass.
    ocCmdLineIterator arg = ocCmdLineBegin(cmdlineWinMain);
    while (ocCmdLineNext(&arg)) {
        cmdlineLen += strlen(arg.value) + 1;    // +1 for null terminator.
        argc += 1;
    }

    if (argc == 0) {
        return 0;
    }


    // The entire data for the command line is stored in a single buffer.
    char* data = (char*)malloc((argc * sizeof(char**)) + (cmdlineLen * sizeof(char)));
    if (data == NULL) {
        return 0;   // Ran out of memory.
    }

    argv = (char**)data;
    char* cmdlineStr = data + (argc * sizeof(char**));
    


    // Second pass.
    argc = 0;
    cmdlineLen = 0;

    arg = ocCmdLineBegin(cmdlineWinMain);
    while (ocCmdLineNext(&arg)) {
        argv[argc] = cmdlineStr + cmdlineLen;
        
        int i = 0;
        while (arg.value[i] != '\0') {
            argv[argc][i] = arg.value[i];
            i += 1;
        }
        argv[argc][i] = '\0';


        cmdlineLen += strlen(arg.value) + 1;    // +1 for null terminator.
        argc += 1;
    }


    *argvOut = argv;
    return argc;
}

void ocFreeArgv(char** argv)
{
    if (argv == NULL) {
        return;
    }

    free(argv);
}


bool ocCmdLineIsSet(int argc, char** argv, const char* arg)
{
    if (argv == NULL || arg == NULL) return false;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], arg) == 0) {
            return true;
        }
    }

    return false;
}