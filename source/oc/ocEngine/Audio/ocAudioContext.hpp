// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocAudioContext
{
    ocEngineContext* pEngine;
    ma_context internalContext;
    ma_device playbackDevice;
};

//
ocResult ocAudioInit(ocEngineContext* pEngine, ocAudioContext* pAudio);

//
void ocAudioUninit(ocAudioContext* pAudio);
