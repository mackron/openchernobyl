// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocAudioContext
{
    ocEngineContext* pEngine;
    dra_context internalContext;
    dra_device playbackDevice;
};

//
ocResult ocAudioInit(ocEngineContext* pEngine, ocAudioContext* pAudio);

//
void ocAudioUninit(ocAudioContext* pAudio);
