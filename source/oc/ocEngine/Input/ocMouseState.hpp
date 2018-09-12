// Copyright (C) 2018 David Reid. See included LICENSE file.

#define OC_MAX_MOUSE_BUTTON_COUNT   16

struct ocMouseState
{
    float absolutePosX;
    float absolutePosY;
    float absolutePosXPrev;
    float absolutePosYPrev;
    float pinnedPosX;
    float pinnedPosY;
    ocUInt16 mouseButtonDownState;          // 1 bit per mouse button. When set it means the button is down. When unset it means the button is released.
    ocUInt16 mouseButtonDownStatePrev;
    ocUInt16 mouseButtonDoubleClickState;   // As above, except for double click.
    ocUInt16 mouseButtonDoubleClickStatePrev;
    ocBool32 isPinned : 1;
};

ocResult ocMouseStateInit(ocMouseState* pState);
void ocMouseStateGetRelativePosition(const ocMouseState* pState, float* pRelativePosX, float* pRelativePosY);
ocFloat32 ocMouseStateGetRelativePositionX(const ocMouseState* pState);
ocFloat32 ocMouseStateGetRelativePositionY(const ocMouseState* pState);
void ocMouseStateGetAbsolutePosition(const ocMouseState* pState, float* pAbsolutePosX, float* pAbsolutePosY);
ocFloat32 ocMouseStateGetAbsolutePositionX(const ocMouseState* pState);
ocFloat32 ocMouseStateGetAbsolutePositionY(const ocMouseState* pState);
ocBool32 ocMouseStateIsButtonDown(const ocMouseState* pState, ocMouseButton button);
ocBool32 ocMouseStateIsButtonUp(const ocMouseState* pState, ocMouseButton button);
ocBool32 ocMouseStateWasButtonPressed(const ocMouseState* pState, ocMouseButton button);
ocBool32 ocMouseStateWasButtonReleased(const ocMouseState* pState, ocMouseButton button);
void ocMouseStateMakeCurrentStatePrevious(ocMouseState* pState);
void ocMouseStateSetAbsolutePosition(ocMouseState* pState, float absolutePosX, float absolutePosY);
void ocMouseStateSetButtonDown(ocMouseState* pState, ocMouseButton button);
void ocMouseStateSetButtonUp(ocMouseState* pState, ocMouseButton button);
void ocMouseStateSetButtonDoubleClicked(ocMouseState* pState, ocMouseButton button);
ocResult ocMouseStatePin(ocMouseState* pState, float absolutePosX, float absolutePosY);
ocResult ocMouseStateUnpin(ocMouseState* pState);