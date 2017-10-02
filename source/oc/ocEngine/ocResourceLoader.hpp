// Copyright (C) 2017 David Reid. See included LICENSE file.

enum ocResourceType
{
    ocResourceType_Unknown,
    ocResourceType_Image,
    ocResourceType_Material,
    ocResourceType_Scene
};

struct ocResourceLoader
{
    ocFileSystem* pFS;
};

// Initializes the resource loader.
ocResult ocResourceLoaderInit(ocFileSystem* pFS, ocResourceLoader* pLoader);

// Uninitializes the resource loader.
void ocResourceLoaderUninit(ocResourceLoader* pLoader);

// Determines the type of resource to load.
//
// This will use the file path to determine the type, but when the path is ambiguous (such as .ocd files) it
// will look at the contents of the file.
ocResult ocResourceLoaderDetermineResourceType(ocResourceLoader* pLoader, const char* filePath, ocResourceType* pType);




///////////////////////////////////////////////////////////////////////////////
//
// Images
//
///////////////////////////////////////////////////////////////////////////////

struct ocImageData
{
    ocImageFormat format;
    uint32_t mipmapCount;
    ocMipmapInfo* pMipmaps;
    size_t imageDataSize;
    void* pImageData;

    // [Internal Use Only] Dynamically allocated data as a single allocation.
    //
    // Format:
    //   sizeof(ocMipmapInfo) * mipmapCount
    //   Raw image data of imageDataSize bytes
    void* _pPayload;
};

// Loads an image.
ocResult ocResourceLoaderLoadImage(ocResourceLoader* pLoader, const char* filePath, ocImageData* pData);

// Unloads an image.
void ocResourceLoaderUnloadImage(ocResourceLoader* pLoader, ocImageData* pData);




///////////////////////////////////////////////////////////////////////////////
//
// Scenes
//
///////////////////////////////////////////////////////////////////////////////

#define OC_SCENE_OBJECT_NONE    (~0UL)

enum ocSceneSubresourceType
{
    ocSceneSubresourceType_Unknown        = 0,
    ocSceneSubresourceType_Scene          = 1,  // Scenes can be contained within scenes. For example, models are just minature scenes, and almost all scenes will include a number of models.
    ocSceneSubresourceType_Material       = 2,
    ocSceneSubresourceType_ParticleSystem = 3
};

struct ocSceneSubresource
{
    ocSceneSubresourceType type;
    const char* pName; // This is an offset of ocSceneData::_pPayloadData.
};

enum ocSceneObjectType
{
    ocSceneObjectType_Unknown = 0,      // Used when the source asset has an object of a particular type that the engine does not support or doesn't care about.
    ocSceneObjectType_Empty   = 1,      // Just an empty node used for hierarchy management.
    ocSceneObjectType_Scene   = 2,      // Usually an object that references a model.
    ocSceneObjectType_Mesh    = 2,      // A single independant polygonal mesh.
    ocSceneObjectType_Light   = 3       // <-- Separate this based on the type of light (point, spot, directional, etc.)?
};

union ocSceneOffsetPtr
{
    uint64_t offset;
    const void* ptr;
};

struct ocSceneObject
{
    ocSceneObjectType type;
    uint64_t dataOffset;    // The offset of the object's data relative to the raw data.

    // Hierarchy information. If any of these are -1 (~0) then it's considred NULL. For example, if parentIndex == ~0 it means
    // the object does not have a parent. Use OC_SCENE_OBJECT_NONE.
    uint32_t parentIndex;
    uint32_t firstChildIndex;
    uint32_t lastChildIndex;
    uint32_t prevSiblingIndex;
    uint32_t nextSiblingIndex;

    // Transformation information.
    float absolutePositionX;
    float absolutePositionY;
    float absolutePoyitionZ;
    float absoluteRotationX;
    float absoluteRotationY;
    float absoluteRotationZ;
    float absoluteRotationW;
    float absoluteScaleX;
    float absoluteScaleY;
    float absoluteScaleZ;

    union
    {
        struct
        {
            uint32_t sceneSubresourceIndex;
        } scene;

        struct
        {
            uint32_t vertexCount;
            ocSceneOffsetPtr pVertexData;
            uint32_t indexCount;
            ocSceneOffsetPtr pIndexData;
            uint32_t materialSubresourceIndex;
        } mesh;

        struct
        {
            int unused;
        } light;
    };
};

struct ocSceneData
{
    uint32_t subresourceCount;
    ocSceneSubresource* pSubresources;  // An offset of _pPayload.

    uint32_t objectCount;
    ocSceneObject* pObjects;            // An offset of _pPayload.

    // [Internal Use Only] Dynamically allocated data as a single allocation.
    //
    // Format:
    //     pSubresources
    //     Subresource paths
    //     pObjects
    //     Object hierarchy
    //     Raw data (mesh data, etc.)
    void* _pPayload;
};

// Loads a scene.
ocResult ocResourceLoaderLoadScene(ocResourceLoader* pLoader, const char* filePath, ocSceneData* pData);

// Unloads a scene.
void ocResourceLoaderUnloadScene(ocResourceLoader* pLoader, ocSceneData* pData);
