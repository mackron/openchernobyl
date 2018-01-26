// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocComponentInit(ocWorldObject* pObject, ocComponentType type, ocComponent* pComponent)
{
    if (pComponent == NULL) return OC_RESULT_INVALID_ARGS;
    ocZeroObject(pComponent);

    if (pObject == NULL) return OC_RESULT_INVALID_ARGS;

    pComponent->pObject = pObject;
    pComponent->type = type;

    return OC_RESULT_SUCCESS;
}

void ocComponentUninit(ocComponent* pComponent)
{
    if (pComponent == NULL) return;
}
