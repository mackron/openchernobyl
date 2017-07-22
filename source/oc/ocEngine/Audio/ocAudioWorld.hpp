// Copyright (C) 2017 David Reid. See included LICENSE file.

struct ocAudioWorld
{
    ocAudioContext* pAudio;
};

//
ocResult ocAudioWorldInit(ocAudioContext* pAudio, ocAudioWorld* pWorld);

//
void ocAudioWorldUninit(ocAudioWorld* pWorld);