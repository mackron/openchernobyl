// Copyright (C) 2018 David Reid. See included LICENSE file.

ocResult ocComponentLightInit(ocWorldObject* pObject, ocLightComponent* pComponent)
{
    ocResult result = ocComponentInit(pObject, OC_COMPONENT_TYPE_LIGHT, pComponent);
    if (result != OC_RESULT_SUCCESS) {
        return result;
    }

    pComponent->color.r = 1;
    pComponent->color.g = 1;
    pComponent->color.b = 1;
    pComponent->color.a = 1;

    return OC_RESULT_SUCCESS;
}

void ocComponentLightUninit(ocLightComponent* pComponent)
{
    ocComponentUninit(pComponent);
}