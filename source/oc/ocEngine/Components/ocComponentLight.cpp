// Copyright (C) 2016 David Reid. See included LICENSE file.

ocResult ocComponentLightInit(ocComponentLight* pComponent, ocWorldObject* pObject)
{
    ocResult result = ocComponentInit(pComponent, pObject, OC_COMPONENT_TYPE_LIGHT);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pComponent->color.r = 1;
    pComponent->color.g = 1;
    pComponent->color.b = 1;
    pComponent->color.a = 1;

    return OC_RESULT_SUCCESS;
}

void ocComponentLightUninit(ocComponentLight* pComponent)
{
    ocComponentUninit(pComponent);
}