// Copyright (C) 2017 David Reid. See included LICENSE file.

// Converts a WinMain style command line to argc/argv. Free the returned array with ocFreeArgv().
int ocWinMainToArgv(const char* cmdlineWinMain, char*** argvOut);

// Frees the memory allocated by onWinMainToArgv().
void ocFreeArgv(char** argv);


// Determines whether or not the given command line argument is present.
ocBool32 ocCmdLineIsSet(int argc, char** argv, const char* arg);
