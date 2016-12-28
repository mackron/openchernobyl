// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocComponentLight : public ocComponent
{
    ocColorF color;
};

//
ocResult ocComponentLightInit(ocWorldObject* pObject, ocComponentLight* pComponent);

//
void ocComponentLightUninit(ocComponentLight* pComponent);