// Copyright (C) 2016 David Reid. See included LICENSE file.

OC_INLINE ocResult ocToResultFromMAL(mal_result resultMAL)
{
    switch (resultMAL)
    {
        case MAL_SUCCESS:       return OC_RESULT_SUCCESS;
        case MAL_INVALID_ARGS:  return OC_RESULT_INVALID_ARGS;
        case MAL_OUT_OF_MEMORY: return OC_RESULT_OUT_OF_MEMORY;
        case MAL_NO_BACKEND:    return OC_RESULT_FAILED_TO_INIT_AUDIO;
        default:                return OC_RESULT_UNKNOWN_ERROR;
    }
}

ocResult ocAudioInit(ocEngineContext* pEngine, ocAudioContext* pAudio)
{
    if (pAudio == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pAudio);

    if (pEngine == NULL) return OC_RESULT_INVALID_ARGS;

    pAudio->pEngine = pEngine;

    mal_result resultMAL = mal_context_init(NULL, 0, &pAudio->internalContext);
    if (resultMAL != MAL_SUCCESS) {
        return ocToResultFromMAL(resultMAL);
    }

    mal_device_config config;
    ocZeroObject(&config);
    config.format             = mal_format_s16;
    config.channels           = 2;
    config.sampleRate         = 44100;
    config.bufferSizeInFrames = 0;
    config.periods            = 0;
    config.onRecvCallback     = NULL;
    config.onSendCallback     = NULL;
    config.onStopCallback     = NULL;
    config.onLogCallback      = NULL;
    resultMAL = mal_device_init(&pAudio->internalContext, mal_device_type_playback, 0, &config, pAudio, &pAudio->playbackDevice);
    if (resultMAL != MAL_SUCCESS) {
        mal_context_uninit(&pAudio->internalContext);
        return ocToResultFromMAL(resultMAL);
    }

    return OC_RESULT_SUCCESS;
}

void ocAudioUninit(ocAudioContext* pAudio)
{
    if (pAudio == NULL) return;
    mal_device_uninit(&pAudio->playbackDevice);
    mal_context_uninit(&pAudio->internalContext);
}