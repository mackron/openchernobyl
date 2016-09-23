// Copyright (C) 2016 David Reid. See included LICENSE file.

// This is main source file for the entire game, and is the only file that needs to be compiled (not including dependencies).

#include "ocGame/ocGame.cpp"

int ocMain(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    // The first thing to do on the Windows build is disable DPI scaling. Not doing this will result in problems with
    // the resolution of the game due to the operating system automatically scaling the window.
#ifdef OC_WIN32
    ocMakeDPIAware_Win32();
#endif

    return ocInitAndRun(argc, argv);    // <-- Implemented in ocGame.cpp
}

int main(int argc, char** argv)
{
    return ocMain(argc, argv);
}

#if defined(OC_WIN32)
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nCmdShow;

    char** argv;
    int argc = ocWinMainToArgv(lpCmdLine, &argv);

    int result = ocMain(argc, argv);

    ocFreeArgv(argv);
    return result;
}
#endif
