// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocInputInit(ocInputState* pState)
{
    if (pState == NULL) {
        return OC_RESULT_INVALID_ARGS;
    }

    ocResult result;

    // Mouse
    result = ocMouseStateInit(&pState->mouseState[0]);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    // Keyboard

    // Joystick

    return OC_RESULT_SUCCESS;
}

void ocInputUninit(ocInputState* pState)
{
    if (pState == NULL) {
        return;
    }

    // No-op for now.
}

void ocInputMakeCurrentStatePrevious(ocInputState* pState)
{
    ocMouseStateMakeCurrentStatePrevious(&pState->mouseState[0]);
}