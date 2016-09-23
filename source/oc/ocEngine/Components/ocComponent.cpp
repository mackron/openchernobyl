// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocComponentInit(ocComponent* pComponent, ocWorldObject* pObject, ocComponentType type)
{
    if (pComponent == NULL || pObject == NULL) return OC_RESULT_INVALID_ARGS;

    ocZeroObject(pComponent);
    pComponent->pObject = pObject;
    pComponent->type = type;

    return OC_RESULT_SUCCESS;
}

void ocComponentUninit(ocComponent* pComponent)
{
    if (pComponent == NULL) return;
}
