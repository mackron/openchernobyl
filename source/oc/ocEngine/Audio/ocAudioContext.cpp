// Copyright (C) 2018 David Reid. See included LICENSE file.

OC_INLINE ocResult ocToResultFromMAL(ma_result resultMAL)
{
    switch (resultMAL)
    {
        case MA_SUCCESS:       return OC_SUCCESS;
        case MA_INVALID_ARGS:  return OC_INVALID_ARGS;
        case MA_OUT_OF_MEMORY: return OC_OUT_OF_MEMORY;
        case MA_NO_BACKEND:    return OC_FAILED_TO_INIT_AUDIO;
        default:                return OC_ERROR;
    }
}

void ocOnSendSamplesMAL(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // TODO: Implement me.
    (void)pDevice;
    (void)pOutput;
    (void)pInput;
    (void)frameCount;
}

void ocOnLogMAL(ma_context* pContext, ma_device* pDevice, ma_uint32 logLevel, const char* pMessage)
{
    (void)pContext;

    ocAudioContext* pAudio = (ocAudioContext*)pDevice->pUserData;
    ocAssert(pAudio != NULL);

    ocLogf(pAudio->pEngine, "AUDIO: [%s] %s", /*ma_log_level_to_string(logLevel)*/ "ERROR", pMessage); (void)logLevel;  // TODO: Replace "ERROR" with ma_log_level_to_string().
}

ocResult ocAudioInit(ocEngineContext* pEngine, ocAudioContext* pAudio)
{
    if (pAudio == NULL) return OC_INVALID_ARGS;
    ocZeroObject(pAudio);

    if (pEngine == NULL) return OC_INVALID_ARGS;

    pAudio->pEngine = pEngine;

    ma_context_config contextConfig = ma_context_config_init();
    contextConfig.logCallback = ocOnLogMAL;

    ma_result resultMAL = ma_context_init(NULL, 0, &contextConfig, &pAudio->internalContext);
    if (resultMAL != MA_SUCCESS) {
        return ocToResultFromMAL(resultMAL);
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = 0;  // <-- Prefer device's native channel count.
    deviceConfig.sampleRate        = 48000;
    deviceConfig.dataCallback      = ocOnSendSamplesMAL;
    deviceConfig.pUserData         = pAudio;

    resultMAL = ma_device_init(&pAudio->internalContext, &deviceConfig, &pAudio->playbackDevice);
    if (resultMAL != MA_SUCCESS) {
        ma_context_uninit(&pAudio->internalContext);
        return ocToResultFromMAL(resultMAL);
    }

    return OC_SUCCESS;
}

void ocAudioUninit(ocAudioContext* pAudio)
{
    if (pAudio == NULL) {
        return;
    }

    ma_device_uninit(&pAudio->playbackDevice);
    ma_context_uninit(&pAudio->internalContext);
}