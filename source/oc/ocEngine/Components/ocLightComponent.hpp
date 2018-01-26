// Copyright (C) 2018 David Reid. See included LICENSE file.

#define OC_LIGHT_COMPONENT(pComponent) ((ocLightComponent*)pComponent)
struct ocLightComponent : public ocComponent
{
    ocColorF color;
};

//
ocResult ocComponentLightInit(ocWorldObject* pObject, ocLightComponent* pComponent);

//
void ocComponentLightUninit(ocLightComponent* pComponent);