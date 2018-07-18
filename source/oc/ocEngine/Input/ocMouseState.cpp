// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocMouseStateInit(ocMouseState* pState)
{
    if (pState == NULL) {
        return OC_RESULT_INVALID_ARGS;
    }

    ocZeroObject(pState);
    return OC_RESULT_SUCCESS;
}

void ocMouseStateGetRelativePosition(const ocMouseState* pState, float* pRelativePosX, float* pRelativePosY)
{
    if (pRelativePosX) *pRelativePosX = 0;
    if (pRelativePosY) *pRelativePosY = 0;

    if (pState == NULL) {
        return;
    }

    if (pRelativePosX) *pRelativePosX = (pState->absolutePosX - pState->absolutePosXPrev);
    if (pRelativePosY) *pRelativePosY = (pState->absolutePosY - pState->absolutePosYPrev);
}

ocFloat32 ocMouseStateGetRelativePositionX(const ocMouseState* pState)
{
    if (pState == NULL) {
        return 0;
    }

    return (pState->absolutePosX - pState->absolutePosXPrev);
}

ocFloat32 ocMouseStateGetRelativePositionY(const ocMouseState* pState)
{
    if (pState == NULL) {
        return 0;
    }

    return (pState->absolutePosY - pState->absolutePosYPrev);
}

void ocMouseStateGetAbsolutePosition(const ocMouseState* pState, float* pAbsolutePosX, float* pAbsolutePosY)
{
    if (pAbsolutePosX) *pAbsolutePosX = 0;
    if (pAbsolutePosY) *pAbsolutePosY = 0;

    if (pState == NULL) {
        return;
    }

    if (pAbsolutePosX) *pAbsolutePosX = pState->absolutePosX;
    if (pAbsolutePosY) *pAbsolutePosY = pState->absolutePosY;
}

ocFloat32 ocMouseStateGetAbsolutePositionX(const ocMouseState* pState)
{
    if (pState == NULL) {
        return 0;
    }

    return pState->absolutePosX;
}

ocFloat32 ocMouseStateGetAbsolutePositionY(const ocMouseState* pState)
{
    if (pState == NULL) {
        return 0;
    }

    return pState->absolutePosY;
}


ocBool32 ocMouseStateIsButtonDown(const ocMouseState* pState, ocMouseButton button)
{
    if (pState == NULL) {
        return OC_FALSE;
    }

    return (pState->mouseButtonDownState & button) != 0;
}

ocBool32 ocMouseStateIsButtonUp(const ocMouseState* pState, ocMouseButton button)
{
    return !ocMouseStateIsButtonDown(pState, button);
}

ocBool32 ocMouseStateWasButtonPressed(const ocMouseState* pState, ocMouseButton button)
{
    if (pState == NULL) {
        return OC_FALSE;
    }

    ocBool32 wasDown = (pState->mouseButtonDownStatePrev & button) != 0;
    ocBool32 isDown = ocMouseStateIsButtonDown(pState, button);

    return !wasDown && isDown;
}

ocBool32 ocMouseStateWasButtonReleased(const ocMouseState* pState, ocMouseButton button)
{
    if (pState == NULL) {
        return OC_FALSE;
    }

    ocBool32 wasDown = (pState->mouseButtonDownStatePrev & button) != 0;
    ocBool32 isDown = ocMouseStateIsButtonDown(pState, button);

    return wasDown && !isDown;
}


void ocMouseStateMakeCurrentStatePrevious(ocMouseState* pState)
{
    if (pState == NULL) {
        return;
    }

    pState->absolutePosXPrev = pState->absolutePosX;
    pState->absolutePosYPrev = pState->absolutePosY;
    pState->mouseButtonDownStatePrev = pState->mouseButtonDownState;
    pState->mouseButtonDoubleClickStatePrev = pState->mouseButtonDoubleClickState;

    // Doesn't make sense for double-click state to be maintained between state movement.
    pState->mouseButtonDownState = 0;
}

void ocMouseStateSetAbsolutePosition(ocMouseState* pState, float absolutePosX, float absolutePosY)
{
    if (pState == NULL) {
        return;
    }

    pState->absolutePosX = absolutePosX;
    pState->absolutePosY = absolutePosY;
}

void ocMouseStateSetButtonDown(ocMouseState* pState, ocMouseButton button)
{
    if (pState == NULL) {
        return;
    }

    pState->mouseButtonDownState |= button;
}

void ocMouseStateSetButtonUp(ocMouseState* pState, ocMouseButton button)
{
    if (pState == NULL) {
        return;
    }

    pState->mouseButtonDownState &= ~button;
}

void ocMouseStateSetButtonDoubleClicked(ocMouseState* pState, ocMouseButton button)
{
    if (pState == NULL) {
        return;
    }

    pState->mouseButtonDoubleClickState |= button;
}
