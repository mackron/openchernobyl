// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocComponentLight : public ocComponent
{
    ocColorF color;
};

//
ocResult ocComponentLightInit(ocComponentLight* pComponent, ocWorldObject* pObject);

//
void ocComponentLightUninit(ocComponentLight* pComponent);