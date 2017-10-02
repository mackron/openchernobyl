// Copyright (C) 2017 David Reid. See included LICENSE file.

// This file contains helpers and utilities for working with OCD files.

#define OC_OCD_FOURCC           0x2044434f /*'OCD ' LE*/
#define OC_OCD_TYPE_ID_IMAGE    0x31474d49 /*'IMG1' LE*/
#define OC_OCD_TYPE_ID_SCENE    0x314e4353 /*'SCN1' LE*/


///////////////////////////////////////////////////////////////////////////////
//
// ocOCDDataBlock
//
///////////////////////////////////////////////////////////////////////////////

struct ocOCDDataBlock : public ocStreamWriter
{
    ocSizeT dataSize;
    void* pData;
};

// Initializes a data block
ocResult ocOCDDataBlockInit(ocOCDDataBlock* pBlock);

// Uninitializes a data block.
ocResult ocOCDDataBlockUninit(ocOCDDataBlock* pBlock);

// Writes data to the data block, returning the offset of the new data within the block.
ocResult ocOCDDataBlockWrite(ocOCDDataBlock* pBlock, const void* pData, ocSizeT dataSize, ocUInt64* pOffsetOut);

// Writes a null-terminated string to the data block.
//
// The data will be padded with zeros to make it 32-bit aligned.
ocResult ocOCDDataBlockWriteString(ocOCDDataBlock* pBlock, const char* pString, ocUInt64* pOffsetOut);

// Writes zero bytes until the buffer is 32-bit aligned.
ocResult ocOCDDAtaBlockWritePadding32(ocOCDDataBlock* pBlock);




///////////////////////////////////////////////////////////////////////////////
//
// ocOCDSceneBuilder
//
///////////////////////////////////////////////////////////////////////////////

struct ocOCDSceneBuilderSubresource
{
    ocUInt64 pathOffset;
    ocUInt32 flags;
    ocUInt64 dataSize;
    ocUInt64 dataOffset;
};

struct ocOCDSceneBuilderObject
{
    ocUInt64 nameOffset;
    ocUInt32 parentIndex;
    ocUInt32 firstChildIndex;
    ocUInt32 lastChildIndex;
    ocUInt32 prevSiblingIndex;
    ocUInt32 nextSiblingIndex;
    glm::vec3 relativePosition;
    glm::quat relativeRotation;
    glm::vec3 relativeScale;
    ocUInt32 componentCount;
    ocUInt64 componentsOffset;
};

struct ocOCDSceneBuilderComponent
{
    ocUInt32 type;
    ocUInt64 dataSize;
    ocUInt64 dataOffset;
};

struct ocOCDSceneBuilder
{
    ocStreamWriter* pWriter;
    ocStack<ocUInt32> objectStack;  // <-- Values are indices into the "objects" list below.
    
    ocStack<ocOCDSceneBuilderSubresource> subresources;
    ocStack<ocOCDSceneBuilderObject> objects;
    ocStack<ocOCDSceneBuilderComponent> components;

    ocOCDDataBlock subresourceBlock;
    ocOCDDataBlock objectBlock;
    ocOCDDataBlock componentBlock;
    ocOCDDataBlock componentDataBlock;
    ocOCDDataBlock subresourceDataBlock;
    ocOCDDataBlock stringDataBlock;
};

// Initializes a scene builder.
ocResult ocOCDSceneBuilderInit(ocStreamWriter* pWriter, ocOCDSceneBuilder* pBuilder);

// Uninitializes a scene builder.
ocResult ocOCDSceneBuilderUninit(ocOCDSceneBuilder* pBuilder);

// Starts a new object.
//
// When called between another BeginObject/EndObject pair, the new object will be a child of that outer object.
ocResult ocOCDSceneBuilderBeginObject(ocOCDSceneBuilder* pBuilder, const char* name, const glm::vec3 &relativePosition, const glm::quat &relativeRotation, const glm::vec3 &relativeScale);

// Ends an object.
ocResult ocOCDSceneBuilderEndObject(ocOCDSceneBuilder* pBuilder);

// Adds a scene component to the current object.
//
// A scene component is used for objects that represent a model, etc. They will always be linked to either an internal or external resource.
ocResult ocOCDSceneBuilderAddSceneComponent(ocOCDSceneBuilder* pBuilder);

// Adds a mesh component to the current object.
ocResult ocOCDSceneBuilderAddMeshComponent(ocOCDSceneBuilder* pBuilder);
