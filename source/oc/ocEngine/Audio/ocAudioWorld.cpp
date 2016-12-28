// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocAudioWorldInit(ocAudioContext* pAudio, ocAudioWorld* pWorld)
{
    if (pWorld == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pWorld);

    if (pAudio == NULL) return OC_RESULT_INVALID_ARGS;

    pWorld->pAudio = pAudio;
    pWorld->pInternalWorld = dra_sound_world_create(&pAudio->playbackDevice);
    if (pWorld->pInternalWorld == NULL) {
        return OC_RESULT_UNKNOWN_ERROR;
    }

    return OC_RESULT_SUCCESS;
}

void ocAudioWorldUninit(ocAudioWorld* pWorld)
{
    if (pWorld == NULL) return;
    dra_sound_world_delete(pWorld->pInternalWorld);
}