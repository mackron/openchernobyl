// Copyright (C) 2017 David Reid. See included LICENSE file.

struct ocAudioContext
{
    ocEngineContext* pEngine;
    mal_context internalContext;
    mal_device playbackDevice;
};

//
ocResult ocAudioInit(ocEngineContext* pEngine, ocAudioContext* pAudio);

//
void ocAudioUninit(ocAudioContext* pAudio);
