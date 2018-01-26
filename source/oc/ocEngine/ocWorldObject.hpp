// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocWorldObject
{
    ocWorld* pWorld;        // <-- Should never be null. Do _not_ use this to determine if the object is in the world - use ocWorldObjectIsInWorld().
    glm::vec4 position;     // <-- Don't set this directly to move the object. Use ocWorldObjectSetPosition().
    glm::quat rotation;     // <-- As above, except ocWorldObjectSetRotation()
    glm::vec4 scale;        // <-- As above, except ocWorldObjectSetScale()
    ocComponent* ppComponents[OC_MAX_COMPONENTS];
    ocUInt16 componentCount;
    ocBool32 isInWorld : 1;
};

// Initializes a world object. This does not add the object to the world - use ocWorldInsertObject() to add do this.
ocResult ocWorldObjectInit(ocWorld* pWorld, ocWorldObject* pObject);

// Uninitializes the object. The object must be removed from the world with ocWorldRemoveObject() beforehand.
void ocWorldObjectUninit(ocWorldObject* pObject);


// A world object isn't inside the world until ocWorldInsertObject() is called. This function determines whether or not
// the object is in the world or not.
ocBool32 ocWorldObjectIsInWorld(ocWorldObject* pObject);


// Creates and adds a new component to an object.
//
// This will fail if the object is already in the world. To add and remove components, you need
// to first remove it from the world, add the component, and then re-add the object to the world.
ocComponent* ocWorldObjectAddComponent(ocWorldObject* pObject, ocComponentType type);

// Removes the component at the given index. This will delete the component.
void ocWorldObjectRemoveComponentByIndex(ocWorldObject* pObject, uint32_t index);

// Removes every component attached to the given world object. This will delete each component.
void ocWorldObjectRemoveAllComponents(ocWorldObject* pObject);


// Sets the position of the world object.
//
// Consider using ocWorldObjectSetTransform() if you need to change the position, rotation and/or scale
// at the same time.
void ocWorldObjectSetPosition(ocWorldObject* pObject, const glm::vec3 &position);

// Sets the rotation of the world object.
//
// Consider using ocWorldObjectSetTransform() if you need to change the position, rotation and/or scale
// at the same time.
void ocWorldObjectSetRotation(ocWorldObject* pObject, const glm::quat &rotation);

// Sets the scale of the world object.
//
// Consider using ocWorldObjectSetTransform() if you need to change the position, rotation and/or scale
// at the same time.
void ocWorldObjectSetScale(ocWorldObject* pObject, const glm::vec3 &scale);

// Sets the position, rotation and scale of the object as a single operation.
void ocWorldObjectSetTransform(ocWorldObject* pObject, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale);