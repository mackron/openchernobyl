// Copyright (C) 2018 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
//
// Win32
//
///////////////////////////////////////////////////////////////////////////////
#ifdef OC_WIN32
static const char* g_OCWndClassName = "OC.WindowClass";
static LRESULT DefaultWindowProcWin32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ocResult ocPlatformLayerInit_Win32(uintptr_t props[])
{
    (void)props;    // Don't need any properties, yet.

    WNDCLASSEXA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.cbWndExtra    = sizeof(void*);
    wc.lpfnWndProc   = (WNDPROC)DefaultWindowProcWin32;
    wc.lpszClassName = g_OCWndClassName;
    wc.hCursor       = LoadCursorA(NULL, MAKEINTRESOURCEA(32512));
    wc.style         = CS_OWNDC | CS_DBLCLKS;
    if (!RegisterClassExA(&wc)) {
        return OC_ERROR;   // Failed to initialize the window class.
    }

    return OC_SUCCESS;
}

void ocPlatformLayerUninit_Win32()
{
    UnregisterClassA(g_OCWndClassName, GetModuleHandleA(NULL));
}


void ocShowSystemCursor_Win32()
{
    while (::ShowCursor(TRUE) < 0);
}

void ocHideSystemCursor_Win32()
{
    while (::ShowCursor(FALSE) >= 0);
}


ocBool32 ocIsWin32MouseButtonKeyCode(WPARAM wParam)
{
    return wParam == VK_LBUTTON || wParam == VK_RBUTTON || wParam == VK_MBUTTON || wParam == VK_XBUTTON1 || wParam == VK_XBUTTON2;
}

ocKey ocKeyFromWin32(WPARAM wParam)
{
    switch (wParam)
    {
    case VK_BACK:   return OC_KEY_BACKSPACE;
    case VK_SHIFT:  return OC_KEY_SHIFT;
    case VK_ESCAPE: return OC_KEY_ESCAPE;
    case VK_PRIOR:  return OC_KEY_PAGE_UP;
    case VK_NEXT:   return OC_KEY_PAGE_DOWN;
    case VK_END:    return OC_KEY_END;
    case VK_HOME:   return OC_KEY_HOME;
    case VK_LEFT:   return OC_KEY_ARROW_LEFT;
    case VK_UP:     return OC_KEY_ARROW_UP;
    case VK_RIGHT:  return OC_KEY_ARROW_RIGHT;
    case VK_DOWN:   return OC_KEY_ARROW_DOWN;
    case VK_DELETE: return OC_KEY_DELETE;
    case VK_F1:     return OC_KEY_F1;
    case VK_F2:     return OC_KEY_F2;
    case VK_F3:     return OC_KEY_F3;
    case VK_F4:     return OC_KEY_F4;
    case VK_F5:     return OC_KEY_F5;
    case VK_F6:     return OC_KEY_F6;
    case VK_F7:     return OC_KEY_F7;
    case VK_F8:     return OC_KEY_F8;
    case VK_F9:     return OC_KEY_F9;
    case VK_F10:    return OC_KEY_F10;
    case VK_F11:    return OC_KEY_F11;
    case VK_F12:    return OC_KEY_F12;

    default: break;
    }

    return (ocKey)wParam;
}

WORD ocKeyToWin32(ocKey key)
{
    switch (key)
    {
    case OC_KEY_BACKSPACE:   return VK_BACK;
    case OC_KEY_SHIFT:       return VK_SHIFT;
    case OC_KEY_ESCAPE:      return VK_ESCAPE;
    case OC_KEY_PAGE_UP:     return VK_PRIOR;
    case OC_KEY_PAGE_DOWN:   return VK_NEXT;
    case OC_KEY_END:         return VK_END;
    case OC_KEY_HOME:        return VK_HOME;
    case OC_KEY_ARROW_LEFT:  return VK_LEFT;
    case OC_KEY_ARROW_UP:    return VK_UP;
    case OC_KEY_ARROW_RIGHT: return VK_RIGHT;
    case OC_KEY_ARROW_DOWN:  return VK_DOWN;
    case OC_KEY_DELETE:      return VK_DELETE;
    case OC_KEY_F1:          return VK_F1;
    case OC_KEY_F2:          return VK_F2;
    case OC_KEY_F3:          return VK_F3;
    case OC_KEY_F4:          return VK_F4;
    case OC_KEY_F5:          return VK_F5;
    case OC_KEY_F6:          return VK_F6;
    case OC_KEY_F7:          return VK_F7;
    case OC_KEY_F8:          return VK_F8;
    case OC_KEY_F9:          return VK_F9;
    case OC_KEY_F10:         return VK_F10;
    case OC_KEY_F11:         return VK_F11;
    case OC_KEY_F12:         return VK_F12;

    default: break;
    }

    return (WORD)key;
}

ocModifierState ocGetModifierKeyState_Win32()
{
    int stateFlags = 0;

    SHORT keyState = GetAsyncKeyState(VK_SHIFT);
    if (keyState & 0x8000) {
        stateFlags |= OC_MODIFIER_STATE_CTRL_DOWN;
    }

    keyState = GetAsyncKeyState(VK_CONTROL);
    if (keyState & 0x8000) {
        stateFlags |= OC_MODIFIER_STATE_SHIFT_DOWN;
    }

    keyState = GetAsyncKeyState(VK_MENU);
    if (keyState & 0x8000) {
        stateFlags |= OC_MODIFIER_STATE_ALT_DOWN;
    }

    return stateFlags;
}

ocModifierState ocGetMouseEventModifierState_Win32(WPARAM wParam)
{
    int stateFlags = 0;

    if ((wParam & MK_LBUTTON) != 0) {
        stateFlags |= OC_MODIFIER_STATE_LMB_DOWN;
    }

    if ((wParam & MK_RBUTTON) != 0) {
        stateFlags |= OC_MODIFIER_STATE_RMB_DOWN;
    }

    if ((wParam & MK_MBUTTON) != 0) {
        stateFlags |= OC_MODIFIER_STATE_MMB_DOWN;
    }

    if ((wParam & MK_XBUTTON1) != 0) {
        stateFlags |= OC_MODIFIER_STATE_MB4_DOWN;
    }

    if ((wParam & MK_XBUTTON2) != 0) {
        stateFlags |= OC_MODIFIER_STATE_MB5_DOWN;
    }


    if ((wParam & MK_CONTROL) != 0) {
        stateFlags |= OC_MODIFIER_STATE_CTRL_DOWN;
    }

    if ((wParam & MK_SHIFT) != 0) {
        stateFlags |= OC_MODIFIER_STATE_SHIFT_DOWN;
    }


    SHORT keyState = GetAsyncKeyState(VK_MENU);
    if (keyState & 0x8000) {
        stateFlags |= OC_MODIFIER_STATE_ALT_DOWN;
    }


    return stateFlags;
}

static LRESULT DefaultWindowProcWin32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ocWindow* pWindow = (ocWindow*)GetWindowLongPtrA(hWnd, 0);
    if (pWindow == NULL) {
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }

    ocWindowEvent e;
    e.pWindow = pWindow;

    switch (msg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_SIZE:
        {
            e.type = OC_WINDOW_EVENT_SIZE;
            e.data.size.width  = LOWORD(lParam);
            e.data.size.height = HIWORD(lParam);
            ocHandleWindowEvent(pWindow->pEngine, e);
        } break;

        case WM_MOVE:
        {
            e.type = OC_WINDOW_EVENT_MOVE;
            e.data.move.x = (int)(short)LOWORD(lParam);
            e.data.move.y = (int)(short)HIWORD(lParam);
            ocHandleWindowEvent(pWindow->pEngine, e);
        } break;

        case WM_MOUSEMOVE:
        {
            e.type = OC_WINDOW_EVENT_MOUSE_MOVE;
            e.data.mouse_move.modifierState = ocGetModifierKeyState_Win32();
            e.data.mouse_move.mousePosX = (int)(short)LOWORD(lParam);
            e.data.mouse_move.mousePosY = (int)(short)HIWORD(lParam);
            ocHandleWindowEvent(pWindow->pEngine, e);
        } break;

        case WM_KEYDOWN:
        {
            if (!ocIsWin32MouseButtonKeyCode(wParam)) {
                ocModifierState modifierState = ocGetModifierKeyState_Win32();
                if ((lParam & (1 << 30)) != 0) {
                    modifierState |= OC_MODIFIER_STATE_AUTO_REPEATED;
                }

                e.type = OC_WINDOW_EVENT_KEY_DOWN;
                e.data.key_down.key = ocKeyFromWin32(wParam);
                e.data.key_down.modifierState = modifierState;
                ocHandleWindowEvent(pWindow->pEngine, e);
            }
        } break;

        default: break;
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

ocBool32 ocWindowInit_Win32(unsigned int resolutionX, unsigned int resolutionY, ocWindow* pWindow)
{
    assert(pWindow != NULL);

    DWORD dwExStyle = 0;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    pWindow->hWnd = CreateWindowExA(dwExStyle, g_OCWndClassName, OC_PRODUCT_NAME, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, resolutionX, resolutionY, NULL, NULL, NULL, NULL);
    if (pWindow->hWnd == NULL) {
        return false;
    }

    // The ocWindow needs to be linked to the Win32 window handle so it can be accessed from the event handler.
    SetWindowLongPtrA(pWindow->hWnd, 0, (LONG_PTR)pWindow);

    // We should have a window, but before showing it we need to make a few small adjustments to the size such that the client size
    // is equal to resolutionX and resolutionY. When we created the window, we specified resolutionX and resolutionY as the dimensions,
    // however this includes the size of the outer border. The outer border should not be included, so we need to stretch the window
    // just a little bit such that the area inside the borders are exactly equal to resolutionX and resolutionY.
    RECT windowRect;
    RECT clientRect;
    GetWindowRect(pWindow->hWnd, &windowRect);
    GetClientRect(pWindow->hWnd, &clientRect);

    int windowWidth  = (int)resolutionX + ((windowRect.right - windowRect.left) - (clientRect.right - clientRect.left));
    int windowHeight = (int)resolutionY + ((windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top));
    SetWindowPos(pWindow->hWnd, NULL, 0, 0, windowWidth, windowHeight, SWP_NOZORDER | SWP_NOMOVE);

    pWindow->hDC = GetDC(pWindow->hWnd);
    return true;
}

void ocWindowUninit_Win32(ocWindow* pWindow)
{
    assert(pWindow != NULL);

    DestroyWindow(pWindow->hWnd);
}

void ocWindowShow_Win32(ocWindow* pWindow)
{
    assert(pWindow != NULL);

    ShowWindow(pWindow->hWnd, SW_SHOW);
}


void ocWindowGetSize_Win32(ocWindow* pWindow, unsigned int* pSizeX, unsigned int* pSizeY)
{
    assert(pWindow != NULL);

    RECT rect;
    GetClientRect(pWindow->hWnd, &rect);

    if (pSizeX != NULL) *pSizeX = rect.right - rect.left;
    if (pSizeY != NULL) *pSizeY = rect.bottom - rect.top;
}

void ocWindowSetSize_Win32(ocWindow* pWindow, unsigned int sizeX, unsigned int sizeY)
{
    assert(pWindow != NULL);

    RECT windowRect;
    RECT clientRect;
    GetWindowRect(pWindow->hWnd, &windowRect);
    GetClientRect(pWindow->hWnd, &clientRect);

    int windowFrameX = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
    int windowFrameY = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

    assert(windowFrameX >= 0);
    assert(windowFrameY >= 0);

    int scaledSizeX = sizeX  + windowFrameX;
    int scaledSizeY = sizeY + windowFrameY;
    SetWindowPos(pWindow->hWnd, NULL, 0, 0, scaledSizeX, scaledSizeY, SWP_NOZORDER | SWP_NOMOVE);
}

ocResult ocWindowCaptureMouse_Win32(ocWindow* pWindow)
{
    ocAssert(pWindow != NULL);

    SetCapture(pWindow->hWnd);
    return OC_SUCCESS;
}

ocResult ocWindowReleaseMouse_Win32(ocWindow* pWindow)
{
    ocAssert(pWindow != NULL);

    if (!ReleaseCapture()) {
        return OC_ERROR;
    }

    return OC_SUCCESS;
}
#endif  // Win32


///////////////////////////////////////////////////////////////////////////////
//
// X11
//
///////////////////////////////////////////////////////////////////////////////
#ifdef OC_X11
static Display* g_X11Display = NULL;
static ocBool32 g_OwnsDisplay = false;
static XVisualInfo* g_VisualInfo = NULL;
static ocBool32 g_OwnsVisualInfo = false;
static Colormap g_Colormap = 0;
static ocBool32 g_OwnsColormap = false;
static Atom g_WM_DELETE_WINDOW = 0;
static Atom g_Atom_ocWindow = 0;

ocResult ocPlatformLayerInit_X11(uintptr_t props[])
{
    // Available properties:
    //   OC_PLATFORM_LAYER_PROP_XDISPLAY
    //   OC_PLATFORM_LAYER_PROP_XVISUALINFO (Required for OpenGL)
    //   OC_PLATFORM_LAYER_PROP_XCOLORMAP (Required for OpenGL)

    if (props != NULL) {
        int iprop = 0;
        for (;;) {
            uintptr_t key = props[iprop++];
            if (key == 0) {
                break;
            }

            uintptr_t val = props[iprop++];
            switch (key) {
                case OC_PLATFORM_LAYER_PROP_XDISPLAY:    g_X11Display = (Display*)val; break;
                case OC_PLATFORM_LAYER_PROP_XVISUALINFO: g_VisualInfo = (XVisualInfo*)val; break;
                case OC_PLATFORM_LAYER_PROP_XCOLORMAP:   g_Colormap = (Colormap)val; break;
                default: break;
            }
        }
    }

    if (g_X11Display == NULL) {
        g_X11Display = XOpenDisplay(NULL);
        if (g_X11Display == NULL) {
            return OC_NO_BACKEND;    // Failed to open a connection to the X display.
        }

        g_OwnsDisplay = true;
    }

    if (g_VisualInfo == NULL) {
        g_VisualInfo = (XVisualInfo*)ocMalloc(sizeof(*g_VisualInfo));
        if (g_VisualInfo == NULL) {
            return OC_OUT_OF_MEMORY;
        }

        int screen = XDefaultScreen(g_X11Display);
        int depth = XDefaultDepth(g_X11Display, screen);
        XMatchVisualInfo(g_X11Display, screen, depth, InputOutput, g_VisualInfo);
        g_OwnsVisualInfo = true;
    }

    if (g_Colormap == 0) {
        g_Colormap = XCreateColormap(g_X11Display, XRootWindow(g_X11Display, g_VisualInfo->screen), g_VisualInfo->visual, AllocNone);
        g_OwnsColormap = true;
    }

    g_WM_DELETE_WINDOW = XInternAtom(g_X11Display, "WM_DELETE_WINDOW", False);
    g_Atom_ocWindow = XInternAtom(g_X11Display, "ATOM_ocWindow", False);

    return OC_SUCCESS;
}

void ocPlatformLayerUninit_X11()
{
    if (g_OwnsDisplay) XCloseDisplay(g_X11Display);
    g_X11Display = NULL;
    g_OwnsDisplay = false;

    if (g_OwnsVisualInfo) ocFree(g_VisualInfo);
    g_VisualInfo = NULL;
    g_OwnsVisualInfo = false;

    if (g_OwnsColormap) XFreeColormap(g_X11Display, g_Colormap);
    g_Colormap = 0;
    g_OwnsVisualInfo = false;
}

OC_PRIVATE void ocSetX11WindowProperty(Display* display, Window window, Atom property, const void* pUserData)
{
    XChangeProperty(display, window, property, XA_INTEGER, 8, PropModeReplace, (const unsigned char*)&pUserData, sizeof(pUserData));
}

OC_PRIVATE void* ocGetX11WindowProperty(Display* display, Window window, Atom property)
{
    Atom actualType;
    int unused1;
    unsigned long unused2;
    unsigned long unused3;

    unsigned char* pRawData;
    XGetWindowProperty(display, window, property, 0, sizeof(void*), False, XA_INTEGER, &actualType, &unused1, &unused2, &unused3, &pRawData);

    void* pUserData;
    memcpy(&pUserData, pRawData, sizeof(pUserData));

    XFree(pRawData);
    return pUserData;
}

ocBool32 ocWindowInit_X11(unsigned int resolutionX, unsigned int resolutionY, ocWindow* pWindow)
{
    assert(pWindow != NULL);

    XSetWindowAttributes attr;
    attr.colormap = g_Colormap;
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | FocusChangeMask | VisibilityChangeMask;

    pWindow->windowX11 = XCreateWindow(g_X11Display, XRootWindow(g_X11Display, g_VisualInfo->screen), 0, 0, resolutionX, resolutionY, 0, g_VisualInfo->depth, InputOutput, g_VisualInfo->visual, CWColormap | CWEventMask, &attr);
    if (pWindow->windowX11 == 0) {
        return false;
    }

    ocSetX11WindowProperty(g_X11Display, pWindow->windowX11, g_Atom_ocWindow, pWindow);

    XSetWMProtocols(g_X11Display, pWindow->windowX11, &g_WM_DELETE_WINDOW, 1);
    XStoreName(g_X11Display, pWindow->windowX11, OC_PRODUCT_NAME);

    return true;
}

void ocWindowUninit_X11(ocWindow* pWindow)
{
    assert(pWindow != NULL);
    XDestroyWindow(g_X11Display, pWindow->windowX11);
}

void ocWindowShow_X11(ocWindow* pWindow)
{
    assert(pWindow != NULL);
    XMapRaised(g_X11Display, pWindow->windowX11);
}


void ocWindowGetSize_X11(ocWindow* pWindow, unsigned int* pSizeX, unsigned int* pSizeY)
{
    if (pSizeX != NULL) *pSizeX = 0;
    if (pSizeY != NULL) *pSizeY = 0;

    Window root;
    int x;
    int y;
    unsigned int border_width;
    unsigned int depth;
    XGetGeometry(g_X11Display, pWindow->windowX11, &root, &x, &y, pSizeX, pSizeY, &border_width, &depth);
}

void ocWindowSetSize_X11(ocWindow* pWindow, unsigned int sizeX, unsigned int sizeY)
{
    XResizeWindow(g_X11Display, pWindow->windowX11, sizeX, sizeY);
}
#endif  // X11


uint32_t g_InitCount = 0;
ocResult ocPlatformLayerInit(uintptr_t props[])
{
    g_InitCount = ocAtomicIncrement(&g_InitCount);
    if (g_InitCount > 1) {
        return OC_SUCCESS;   // Already initialized.
    }

#ifdef OC_WIN32
    return ocPlatformLayerInit_Win32(props);
#endif
#ifdef OC_X11
    return ocPlatformLayerInit_X11(props);
#endif
}

void ocPlatformLayerUninit()
{
    g_InitCount = ocAtomicDecrement(&g_InitCount);
    if (g_InitCount > 0) {
        return; // Not the last uninit, so just return early.
    }

#ifdef OC_WIN32
    return ocPlatformLayerUninit_Win32();
#endif
#ifdef OC_X11
    return ocPlatformLayerUninit_X11();
#endif
}


void ocShowSystemCursor()
{
#ifdef OC_WIN32
    ocShowSystemCursor_Win32();
#endif
#ifdef OC_X11
    ocShowSystemCursor_X11();
#endif
}

void ocHideSystemCursor()
{
#ifdef OC_WIN32
    ocHideSystemCursor_Win32();
#endif
#ifdef OC_X11
    ocHideSystemCursor_X11();
#endif
}


ocBool32 ocWindowInit(ocEngineContext* pEngine, unsigned int resolutionX, unsigned int resolutionY, ocWindow* pWindow)
{
    if (pWindow == NULL) return false;
    ocZeroObject(pWindow);

    pWindow->pEngine = pEngine;

#ifdef OC_WIN32
    return ocWindowInit_Win32(resolutionX, resolutionY, pWindow);
#endif
#ifdef OC_X11
    return ocWindowInit_X11(resolutionX, resolutionY, pWindow);
#endif
}

void ocWindowUninit(ocWindow* pWindow)
{
    if (pWindow == NULL) return;

#ifdef OC_WIN32
    ocWindowUninit_Win32(pWindow);
#endif
#ifdef OC_X11
    ocWindowUninit_X11(pWindow);
#endif
}

void ocWindowShow(ocWindow* pWindow)
{
    if (pWindow == NULL) return;

#ifdef OC_WIN32
    ocWindowShow_Win32(pWindow);
#endif
#ifdef OC_X11
    ocWindowShow_X11(pWindow);
#endif
}


void ocWindowGetSize(ocWindow* pWindow, unsigned int* pSizeX, unsigned int* pSizeY)
{
    if (pWindow == NULL) return;

#ifdef OC_WIN32
    ocWindowGetSize_Win32(pWindow, pSizeX, pSizeY);
#endif
#ifdef OC_X11
    ocWindowGetSize_X11(pWindow, pSizeX, pSizeY);
#endif
}

void ocWindowSetSize(ocWindow* pWindow, unsigned int sizeX, unsigned int sizeY)
{
    if (pWindow == NULL) return;

    // Clamp to 1x1.
    if (sizeX == 0) sizeX = 1;
    if (sizeY == 0) sizeY = 1;

#ifdef OC_WIN32
    ocWindowSetSize_Win32(pWindow, sizeX, sizeY);
#endif
#ifdef OC_X11
    ocWindowSetSize_X11(pWindow, sizeX, sizeY);
#endif
}


ocResult ocWindowCaptureMouse(ocWindow* pWindow)
{
    if (pWindow == NULL) {
        return OC_INVALID_ARGS;
    }

#ifdef OC_WIN32
    return ocWindowCaptureMouse_Win32(pWindow);
#endif
#ifdef OC_X11
    return ocWindowCaptureMouse_X11(pWindow);
#endif
}

ocResult ocWindowReleaseMouse(ocWindow* pWindow)
{
    if (pWindow == NULL) {
        return OC_INVALID_ARGS;
    }

#ifdef OC_WIN32
    return ocWindowReleaseMouse_Win32(pWindow);
#endif
#ifdef OC_X11
    return ocWindowReleaseMouse_X11(pWindow);
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
// Timing
//
///////////////////////////////////////////////////////////////////////////////
#ifdef OC_WIN32
static LARGE_INTEGER g_OCTimerFrequency = {0};

void ocTimerInit_Win32(ocTimer* pTimer)
{
    assert(pTimer != NULL);

    if (g_OCTimerFrequency.QuadPart == 0) {
        QueryPerformanceFrequency(&g_OCTimerFrequency);
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    pTimer->counter = (uint64_t)counter.QuadPart;
}

double ocTimerTick_Win32(ocTimer* pTimer)
{
    assert(pTimer != NULL);

    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
        return 0;
    }

    uint64_t newTimeCounter = counter.QuadPart;
    uint64_t oldTimeCounter = pTimer->counter;

    pTimer->counter = newTimeCounter;

    return (newTimeCounter - oldTimeCounter) / (double)g_OCTimerFrequency.QuadPart;
}
#endif  // Win32

#ifdef OC_X11
void ocTimerInit_Linux(ocTimer* pTimer)
{
    assert(pTimer != NULL);

    struct timespec newTime;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &newTime);

    pTimer->counter = (newTime.tv_sec * 1000000000LL) + newTime.tv_nsec;
}

double ocTimerTick_Linux(ocTimer* pTimer)
{
    assert(pTimer != NULL);

    struct timespec newTime;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &newTime);

    uint64_t newTimeCounter = (newTime.tv_sec * 1000000000LL) + newTime.tv_nsec;
    uint64_t oldTimeCounter = pTimer->counter;

    pTimer->counter = newTimeCounter;

    return (newTimeCounter - oldTimeCounter) / 1000000000.0;
}
#endif  // Linux

void ocTimerInit(ocTimer* pTimer)
{
    if (pTimer == NULL) {
        return;
    }

#ifdef OC_WIN32
    ocTimerInit_Win32(pTimer);
#endif

#ifdef OC_X11
    ocTimerInit_Linux(pTimer);
#endif
}

double ocTimerTick(ocTimer* pTimer)
{
    if (pTimer == NULL) {
        return 0;
    }

#ifdef OC_WIN32
    return ocTimerTick_Win32(pTimer);
#endif

#ifdef OC_X11
    return ocTimerTick_Linux(pTimer);
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
// Misc Functions
//
///////////////////////////////////////////////////////////////////////////////

int ocSystem(const char* cmd)
{
    return system(cmd);
}

#ifdef OC_WIN32
int ocMainLoop_Win32(ocEngineContext* pEngine)
{
    for (;;) {
        // Handle window events.
        MSG msg;
        if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return (int)msg.wParam;  // Received a quit message.
            }

            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        // After handling the next event in the queue we let the game know it should do the next frame.
        ocStep(pEngine);
    }
}
#endif  // Win32

#ifdef OC_X11
void ocHandleX11Event(XEvent* ex)
{
    ocWindowEvent e;
    e.pWindow = (ocWindow*)ocGetX11WindowProperty(g_X11Display, ex->xany.window, g_Atom_ocWindow);

    switch (ex->type)
    {
        case ConfigureNotify:
        {
            // TODO: The one event type is used for both movement and resizing. Need to track which one changed, and post one or the other.
            // For the moment, just post both move and size events.
            e.type = OC_WINDOW_EVENT_SIZE;
            e.data.size.width = ex->xconfigure.width;
            e.data.size.height = ex->xconfigure.height;
            ocHandleWindowEvent(e.pWindow->pEngine, e);
        } break;


        case MotionNotify:
        {
        } break;

        case ButtonPress:
        {
        } break;

        case ButtonRelease:
        {
        } break;


        case KeyPress:
        {

        } break;

        case KeyRelease:
        {
        } break;


        default: break;
    }
}

int ocMainLoop_X11(ocEngineContext* pEngine)
{
    for (;;) {
        if (XPending(g_X11Display) > 0) {   // <-- Use a while loop instead?
            XEvent x11Event;
            XNextEvent(g_X11Display, &x11Event);

            if (x11Event.type == ClientMessage) {
                if ((Atom)x11Event.xclient.data.l[0] == g_WM_DELETE_WINDOW) {
                    return 0;   // Received a quit message.
                }
            };

            ocHandleX11Event(&x11Event);
        }

        ocStep(pEngine);
    }
}
#endif  // X11

int ocMainLoop(ocEngineContext* pEngine)
{
#ifdef OC_WIN32
    return ocMainLoop_Win32(pEngine);
#endif

#ifdef OC_X11
    return ocMainLoop_X11(pEngine);
#endif
}
