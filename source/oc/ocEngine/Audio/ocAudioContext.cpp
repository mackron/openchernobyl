// Copyright (C) 2018 David Reid. See included LICENSE file.

OC_INLINE ocResult ocToResultFromMAL(mal_result resultMAL)
{
    switch (resultMAL)
    {
        case MAL_SUCCESS:       return OC_SUCCESS;
        case MAL_INVALID_ARGS:  return OC_INVALID_ARGS;
        case MAL_OUT_OF_MEMORY: return OC_OUT_OF_MEMORY;
        case MAL_NO_BACKEND:    return OC_FAILED_TO_INIT_AUDIO;
        default:                return OC_ERROR;
    }
}

mal_uint32 ocOnSendSamplesMAL(mal_device* pDevice, mal_uint32 frameCount, void* pSamples)
{
    // TODO: Implement me.
    (void)pDevice;
    (void)frameCount;
    (void)pSamples;
    return 0;
}

void ocOnLogMAL(mal_context* pContext, mal_device* pDevice, const char* pMessage)
{
    (void)pContext;

    ocAudioContext* pAudio = (ocAudioContext*)pDevice->pUserData;
    ocAssert(pAudio != NULL);

    ocLogf(pAudio->pEngine, "AUDIO: %s", pMessage);
}

ocResult ocAudioInit(ocEngineContext* pEngine, ocAudioContext* pAudio)
{
    if (pAudio == NULL) return OC_INVALID_ARGS;
    ocZeroObject(pAudio);

    if (pEngine == NULL) return OC_INVALID_ARGS;

    pAudio->pEngine = pEngine;

    mal_context_config contextConfig = mal_context_config_init(ocOnLogMAL);

    mal_result resultMAL = mal_context_init(NULL, 0, &contextConfig, &pAudio->internalContext);
    if (resultMAL != MAL_SUCCESS) {
        return ocToResultFromMAL(resultMAL);
    }

    mal_device_config deviceConfig = mal_device_config_init_playback(
        mal_format_f32,
        0,  // <-- Prefer device's native channel count.
        48000,
        ocOnSendSamplesMAL
    );

    resultMAL = mal_device_init(&pAudio->internalContext, mal_device_type_playback, NULL, &deviceConfig, pAudio, &pAudio->playbackDevice);
    if (resultMAL != MAL_SUCCESS) {
        mal_context_uninit(&pAudio->internalContext);
        return ocToResultFromMAL(resultMAL);
    }

    return OC_SUCCESS;
}

void ocAudioUninit(ocAudioContext* pAudio)
{
    if (pAudio == NULL) return;
    mal_device_uninit(&pAudio->playbackDevice);
    mal_context_uninit(&pAudio->internalContext);
}