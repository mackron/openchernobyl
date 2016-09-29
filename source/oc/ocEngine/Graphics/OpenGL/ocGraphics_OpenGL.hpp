// Copyright (C) 2016 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
//
// GraphicsContext
//
///////////////////////////////////////////////////////////////////////////////

struct ocGraphicsContext : public ocGraphicsContextBase
{
    drgl gl;
    ocWindow* pCurrentWindow;
    uint32_t msaaSamples;
};

struct ocGraphicsFB
{
    GLuint objectGL;                        // The OpenGL framebuffer object.
    GLuint colorRenderbufferGL;
    GLuint depthStencilRenderbufferGL;      // The depth/stencil OpenGL renderbuffer object.
};

struct ocGraphicsImage
{
    GLuint objectGL;    // The OpenGL texture object.
    uint32_t sizeX;
    uint32_t sizeY;
};

struct ocGraphicsMesh
{
    GLuint objectGL;
    ocGraphicsVertexFormat format;
    uint32_t vertexCount;
    uint32_t indexCount;
    GLuint vbo;
    GLuint ibo;
};



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsSwapchain
//
///////////////////////////////////////////////////////////////////////////////

struct ocGraphicsSwapchain : public ocGraphicsSwapchainBase
{
};



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsWorld
//
///////////////////////////////////////////////////////////////////////////////

struct ocGraphicsObject : public ocGraphicsObjectBase
{
    glm::vec4 _position;
    glm::quat _rotation;
    glm::vec4 _scale;
    glm::mat4 _transform;   // <-- The transformation matrix made up of _position, _rotation and _scale

    union
    {
        struct
        {
            ocGraphicsMesh* pResource;
        } mesh;

        struct
        {
            //ocGraphicsResource_ParticleSystem* pResource;
            int unused;
        } particleSystem;

        struct
        {
            ocColorF color;
        } ambientLight;

        struct
        {
            ocColorF color;
            glm::vec3 direction;
        } directionalLight;

        struct
        {
            ocColorF color;
            float radius;
            float attenuation;
        } pointLight;

        struct
        {
            ocColorF color;
            float radius;
            float attenuation;
            float angleOuter;
            float angleInner;
        } spotLight;
    } data;
};

struct ocGraphicsWorld : public ocGraphicsWorldBase
{
    ocGraphicsRT* pRenderTargets[OC_MAX_RENDER_TARGETS];
    uint32_t renderTargetCount;

    GLuint testProgramGL;

    // The list containing the current mesh objects. This is only temporary until a more efficient data structure is implemented. Needs to be a pointer
    // because we initialize to 0.
    std::vector<ocGraphicsObject*>* pObjects;


    // TEMP
    ocGraphicsImage* pCurrentImage;
};





///////////////////////////////////////////////////////////////////////////////
//
// GraphicsRT
//
///////////////////////////////////////////////////////////////////////////////

struct ocGraphicsRT : public ocGraphicsRTBase
{
    ocGraphicsSwapchain* pSwapchain;
    ocGraphicsImage* pImage;
    ocGraphicsFB framebuffer;
    GLsizei sizeX;
    GLsizei sizeY;
    glm::mat4 projection;
    glm::mat4 view;
};