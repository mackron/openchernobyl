// Copyright (C) 2016 David Reid. See included LICENSE file.

#define OC_WORLD_OBJECT_FLAG_IN_WORLD     (1 << 0)

struct ocWorldObject
{
    ocWorld* pWorld;        // <-- Should never be null. Do _not_ use this to determine if the object is in the world - use ocWorldObjectIsInWorld().
    glm::vec4 position;
    glm::quat rotation;
    glm::vec4 scale;
    ocComponent* ppComponents[OC_MAX_COMPONENTS];
    uint16_t componentCount;
    uint16_t flags;
};

// Initializes a world object. This does not add the object to the world - use ocWorldInsertObject() to add do this.
ocResult ocWorldObjectInit(ocWorldObject* pObject, ocWorld* pWorld);

// Uninitializes the object. The object must be removed from the world with ocWorldRemoveObject() beforehand.
void ocWorldObjectUninit(ocWorldObject* pObject);


// A world object isn't inside the world until ocWorldInsertObject() is called. This function determines whether or not
// the object is in the world or not.
dr_bool32 ocWorldObjectIsInWorld(ocWorldObject* pObject);


// Removes the component at the given index. This will delete the component.
void ocWorldObjectRemoveComponentByIndex(ocWorldObject* pObject, uint32_t index);

// Removes every component attached to the given world object. This will delete each component.
void ocWorldObjectRemoveAllComponents(ocWorldObject* pObject);