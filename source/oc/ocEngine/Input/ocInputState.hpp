// Copyright (C) 2018 David Reid. See included LICENSE file.

#define OC_MAX_MOUSE_COUNT      1
#define OC_MAX_KEYBOARD_COUNT   1
#define OC_MAX_JOYSTICK_COUNT   4

struct ocInputState
{
    ocMouseState mouseState[OC_MAX_MOUSE_COUNT];
};

ocResult ocInputInit(ocInputState* pState);
void ocInputUninit(ocInputState* pState);
void ocInputMakeCurrentStatePrevious(ocInputState* pState);