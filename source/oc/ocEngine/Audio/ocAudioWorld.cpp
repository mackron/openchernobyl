// Copyright (C) 2017 David Reid. See included LICENSE file.

ocResult ocAudioWorldInit(ocAudioContext* pAudio, ocAudioWorld* pWorld)
{
    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pWorld);

    if (pAudio == NULL) return OC_RESULT_INVALID_ARGS;

    pWorld->pAudio = pAudio;

    return OC_RESULT_SUCCESS;
}

void ocAudioWorldUninit(ocAudioWorld* pWorld)
{
    if (pWorld == NULL) return;
}