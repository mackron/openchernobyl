// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocWorldObject;

// Position, rotation and scale are all absolute. The reason for this is that storing them as relative
// would require a traversal up the hierarchy to calculate the absolute position, whereas storing them
// as absolute means no calculations need to be performed, and calculating the relative position of an
// object is a simple matter of performing a subtraction (no traversal required).
struct ocWorldObject
{
    ocWorld* pWorld;            // <-- Should never be null. Do _not_ use this to determine if the object is in the world - use ocWorldObjectIsInWorld().
    ocString name;              // <-- Does not need to be unique, and can be null.
    glm::vec4 absolutePosition; // <-- Don't set this directly to move the object. Use ocWorldObjectSetAbsolutePosition().
    glm::quat absoluteRotation; // <-- As above, except ocWorldObjectSetAbsoluteRotation()
    glm::vec4 absoluteScale;    // <-- As above, except ocWorldObjectSetAbsoluteScale()
    ocComponent* ppComponents[OC_MAX_COMPONENTS];
    ocUInt16 componentCount;
    ocUInt16 isInWorld : 1;
    ocWorldObject* pParent;
    ocWorldObject* pFirstChild;
    ocWorldObject* pLastChild;
    ocWorldObject* pPrevSibling;
    ocWorldObject* pNextSibling;
};

// Initializes a world object. This does not add the object to the world - use ocWorldInsertObject() to add do this.
ocResult ocWorldObjectInit(ocWorld* pWorld, ocWorldObject* pObject);

// Uninitializes the object. The object must be removed from the world with ocWorldRemoveObject() beforehand.
void ocWorldObjectUninit(ocWorldObject* pObject);


// A world object isn't inside the world until ocWorldInsertObject() is called. This function determines whether or not
// the object is in the world or not.
ocBool32 ocWorldObjectIsInWorld(ocWorldObject* pObject);


// Sets the name of the object.
ocResult ocWorldObjectSetName(ocWorldObject* pObject, const char* name);

// Retrieves the name of the object.
const char* ocWorldObjectGetName(ocWorldObject* pObject);


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
// Consider using ocWorldObjectSetAbsoluteTransform() if you need to change the position, rotation and/or scale
// at the same time.
void ocWorldObjectSetAbsolutePosition(ocWorldObject* pObject, const glm::vec3 &absolutePosition);

// Sets the rotation of the world object.
//
// Consider using ocWorldObjectSetAbsoluteTransform() if you need to change the position, rotation and/or scale
// at the same time.
void ocWorldObjectSetAbsoluteRotation(ocWorldObject* pObject, const glm::quat &absoluteRotation);

// Sets the scale of the world object.
//
// Consider using ocWorldObjectSetTransform() if you need to change the position, rotation and/or scale
// at the same time.
void ocWorldObjectSetAbsoluteScale(ocWorldObject* pObject, const glm::vec3 &absoluteScale);

// Sets the position, rotation and scale of the object as a single operation.
void ocWorldObjectSetAbsoluteTransform(ocWorldObject* pObject, const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale);