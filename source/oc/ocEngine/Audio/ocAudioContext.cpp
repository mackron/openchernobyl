// Copyright (C) 2016 David Reid. See included LICENSE file.

OC_INLINE ocResult ocToResultFromDRA(dra_result draresult)
{
    switch (draresult)
    {
        case DRA_RESULT_SUCCESS:        return OC_RESULT_SUCCESS;
        case DRA_RESULT_INVALID_ARGS:   return OC_RESULT_INVALID_ARGS;
        case DRA_RESULT_OUT_OF_MEMORY:  return OC_RESULT_OUT_OF_MEMORY;
        case DRA_RESULT_NO_BACKEND:     return OC_RESULT_FAILED_TO_INIT_AUDIO;
        default:                        return OC_RESULT_UNKNOWN_ERROR;
    }
}

ocResult ocAudioInit(ocEngineContext* pEngine, ocAudioContext* pAudio)
{
    if (pAudio == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pAudio);

    if (pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    pAudio->pEngine = pEngine;

    dra_result draresult = dra_context_init(&pAudio->internalContext);
    if (draresult != DRA_RESULT_SUCCESS) {
        return ocToResultFromDRA(draresult);
    }

    draresult = dra_device_init_ex(&pAudio->internalContext, dra_device_type_playback, 0, 2, 48000, 0, &pAudio->playbackDevice);
    if (draresult != DRA_RESULT_SUCCESS) {
        dra_context_uninit(&pAudio->internalContext);
        return ocToResultFromDRA(draresult);
    }

    return OC_RESULT_SUCCESS;
}

void ocAudioUninit(ocAudioContext* pAudio)
{
    if (pAudio == NULL) return;
    dra_device_uninit(&pAudio->playbackDevice);
    dra_context_uninit(&pAudio->internalContext);
}