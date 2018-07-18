// Copyright (C) 2018 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
//
// Window
//
///////////////////////////////////////////////////////////////////////////////

typedef ocUInt32 ocWindowEventType;
#define OC_WINDOW_EVENT_SIZE                    1
#define OC_WINDOW_EVENT_MOVE                    2
#define OC_WINDOW_EVENT_MOUSE_MOVE              3
#define OC_WINDOW_EVENT_MOUSE_BUTTON_DOWN       4
#define OC_WINDOW_EVENT_MOUSE_BUTTON_UP         5
#define OC_WINDOW_EVENT_MOUSE_BUTTON_DBLCLICK   6
#define OC_WINDOW_EVENT_KEY_DOWN                7
#define OC_WINDOW_EVENT_PRINTABLE_KEY_DOWN      8
#define OC_WINDOW_EVENT_KEY_UP                  9

typedef ocUInt32 ocModifierState;
#define OC_MODIFIER_STATE_CTRL_DOWN             (1U << 0U)
#define OC_MODIFIER_STATE_SHIFT_DOWN            (1U << 1U)
#define OC_MODIFIER_STATE_ALT_DOWN              (1U << 2U)
#define OC_MODIFIER_STATE_LMB_DOWN              (1U << 3U)
#define OC_MODIFIER_STATE_RMB_DOWN              (1U << 4U)
#define OC_MODIFIER_STATE_MMB_DOWN              (1U << 5U)
#define OC_MODIFIER_STATE_MB4_DOWN              (1U << 6U)
#define OC_MODIFIER_STATE_MB5_DOWN              (1U << 7U)
#define OC_MODIFIER_STATE_AUTO_REPEATED         (1U << 31U)

typedef ocUInt32 ocMouseButton;
#define OC_MOUSE_BUTTON_LEFT        1
#define OC_MOUSE_BUTTON_RIGHT       2
#define OC_MOUSE_BUTTON_MIDDLE      4
#define OC_MOUSE_BUTTON_4           8
#define OC_MOUSE_BUTTON_5           16
#define OC_MOUSE_BUTTON_6           32
#define OC_MOUSE_BUTTON_7           64

typedef ocUInt32 ocKey;
#define OC_KEY_BACKSPACE              0xff08
#define OC_KEY_SHIFT                  0xff10
#define OC_KEY_ESCAPE                 0xff1b
#define OC_KEY_PAGE_UP                0xff55
#define OC_KEY_PAGE_DOWN              0xff56
#define OC_KEY_END                    0xff57
#define OC_KEY_HOME                   0xff50
#define OC_KEY_ARROW_LEFT             0x8fb
#define OC_KEY_ARROW_UP               0x8fc
#define OC_KEY_ARROW_RIGHT            0x8fd
#define OC_KEY_ARROW_DOWN             0x8fe
#define OC_KEY_DELETE                 0xffff
#define OC_KEY_F1                     0xffbe
#define OC_KEY_F2                     0xffbf
#define OC_KEY_F3                     0xffc0
#define OC_KEY_F4                     0xffc1
#define OC_KEY_F5                     0xffc2
#define OC_KEY_F6                     0xffc3
#define OC_KEY_F7                     0xffc4
#define OC_KEY_F8                     0xffc5
#define OC_KEY_F9                     0xffc6
#define OC_KEY_F10                    0xffc7
#define OC_KEY_F11                    0xffc8
#define OC_KEY_F12                    0xffc9

// Platform Layer Properties.
#ifdef OC_X11
#define OC_PLATFORM_LAYER_PROP_XDISPLAY     1   // Value = Display*
#define OC_PLATFORM_LAYER_PROP_XVISUALINFO  2   // Value = XVisualInfo*
#define OC_PLATFORM_LAYER_PROP_XCOLORMAP    3   // Value = Colormap
#endif

// Initializes the platform layer.
//
// This can be called multiple times, but each call must be matched up with a call to ocPlatformLayerUninit(). Do not call this
// with different values for "props".
ocResult ocPlatformLayerInit(uintptr_t props[]);

// Uninitializes the platform layer.
void ocPlatformLayerUninit();


struct ocWindow
{
    ocEngineContext* pEngine;

#ifdef OC_WIN32
    HWND hWnd;
    HDC hDC;
#endif

#ifdef OC_X11
    Window windowX11;
#endif
};

struct ocWindowEvent
{
    ocWindowEventType type;
    ocWindow* pWindow;

    union
    {
        struct
        {
            ocUInt32 width;
            ocUInt32 height;
        } size;

        struct
        {
            ocInt32 x;
            ocInt32 y;
        } move;

        struct
        {
            ocInt32 mousePosX;
            ocInt32 mousePosY;
            ocModifierState modifierState;
        } mouse_move;

        struct
        {
            ocInt32 mousePosX;
            ocInt32 mousePosY;
            ocMouseButton mouseButton;
            ocModifierState modifierState;
        } mouse_button_down, mouse_button_up, mouse_button_dblclick;

        struct
        {
            ocKey key;
            ocModifierState modifierState;
        } key_down;

        struct
        {
            ocUInt32 utf32;
            ocModifierState modifierState;
        } printable_key_down;

        struct
        {
            ocKey key;
            ocModifierState modifierState;
        } key_up;
    } data;
};


ocBool32 ocWindowInit(ocEngineContext* pEngine, unsigned int resolutionX, unsigned int resolutionY, ocWindow* pWindow);
void ocWindowUninit(ocWindow* pWindow);
void ocWindowShow(ocWindow* pWindow);

void ocWindowGetSize(ocWindow* pWindow, unsigned int* pSizeX, unsigned int* pSizeY);
void ocWindowSetSize(ocWindow* pWindow, unsigned int sizeX, unsigned int sizeY);



///////////////////////////////////////////////////////////////////////////////
//
// Timing
//
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    ocInt64 counter;
} ocTimer;

// Initializes a high-resolution timer.
void ocTimerInit(ocTimer* pTimer);

// Ticks the timer and returns the number of seconds since the previous tick.
//
// The maximum return value is about 140 years or so.
double ocTimerTick(ocTimer* pTimer);



///////////////////////////////////////////////////////////////////////////////
//
// Misc Functions
//
///////////////////////////////////////////////////////////////////////////////

// Executes a system command.
int ocSystem(const char* cmd);

// Runs the main loop. This is where ocStep() will be called from.
int ocMainLoop(ocEngineContext* pEngine);