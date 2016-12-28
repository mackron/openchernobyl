// Copyright (C) 2016 David Reid. See included LICENSE file.

struct ocComponent
{
    ocWorldObject* pObject;     // The world object this component is attached to. Should never be null.
    ocComponentType type;
};

//
ocResult ocComponentInit(ocWorldObject* pObject, ocComponentType type, ocComponent* pComponent);

//
void ocComponentUninit(ocComponent* pComponent);