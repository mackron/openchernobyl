// Copyright (C) 2017 David Reid. See included LICENSE file.

// This file contains helpers and utilities for working with OCD files.

#define OC_OCD_FOURCC           0x2044434f /*'OCD ' LE*/
#define OC_OCD_TYPE_ID_IMAGE    0x31474d49 /*'IMG1' LE*/
#define OC_OCD_TYPE_ID_SCENE    0x314e4353 /*'SCN1' LE*/

///////////////////////////////////////////////////////////////////////////////
//
// ocOCDSceneBuilder
//
///////////////////////////////////////////////////////////////////////////////
struct ocOCDSceneBuilderObject
{
    glm::vec3 relativePosition;
    glm::quat relativeRotation;
    glm::vec3 relativeScale;
};

struct ocOCDSceneBuilder
{
    ocStreamWriter* pWriter;
    ocStack<ocOCDSceneBuilderObject*> objectStack;
};

//
ocResult ocOCDSceneBuilderInit(ocStreamWriter* pWriter, ocOCDSceneBuilder* pBuilder);

//
ocResult ocOCDSceneBuilderUninit(ocOCDSceneBuilder* pBuilder);

// Starts a new object.
//
// When called between another BeginObject/EndObject pair, the new object will be a child of that outer object.
ocResult ocOCDSceneBuilderBeginObject(ocOCDSceneBuilder* pBuilder);

// Ends an object.
ocResult ocOCDSceneBuilderEndObject(ocOCDSceneBuilder* pBuilder);

// Sets the transform of the current object.
ocResult ocOCDSceneBuilderSetRelativeTransform(ocOCDSceneBuilder* pBuilder, const glm::vec3 &relativePosition, const glm::quat &relativeRotation, const glm::vec3 &relativeScale);

// Adds a scene component to the current object.
//
// A scene component is used for objects that represent a model, etc. They will always be linked to either an internal or external resource.
ocResult ocOCDSceneBuilderAddSceneComponent(ocOCDSceneBuilder* pBuilder);

// Adds a mesh component to the current object.
ocResult ocOCDSceneBuilderAddMeshComponent(ocOCDSceneBuilder* pBuilder);
