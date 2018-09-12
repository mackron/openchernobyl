// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocAudioWorldInit(ocAudioContext* pAudio, ocAudioWorld* pWorld)
{
    if (pWorld == NULL) return OC_INVALID_ARGS;
    ocZeroObject(pWorld);

    if (pAudio == NULL) return OC_INVALID_ARGS;

    pWorld->pAudio = pAudio;

    return OC_SUCCESS;
}

void ocAudioWorldUninit(ocAudioWorld* pWorld)
{
    if (pWorld == NULL) return;
}

void ocAudioWorldStep(ocAudioWorld* pWorld, double dt)
{
    if (pWorld == NULL) return;

    (void)dt;
}