// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocAudioWorld
{
    ocAudioContext* pAudio;
    dra_sound_world* pInternalWorld;
};

//
ocResult ocAudioWorldInit(ocAudioWorld* pWorld, ocAudioContext* pAudio);

//
void ocAudioWorldUninit(ocAudioWorld* pWorld);