// Copyright (C) 2017 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
//
// GraphicsContext
//
///////////////////////////////////////////////////////////////////////////////

struct ocGraphicsContext : public ocGraphicsContextBase
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkPhysicalDeviceProperties deviceProps;
    uint32_t queueFamilyIndex;
    uint32_t queueLocalIndex;
    VkCommandPool commandPool;
    VkQueue queue;
    VkDescriptorPool descriptorPool;
    VkRenderPass renderPass0;
    VkRenderPass renderPass_FinalComposite_Image;
    VkRenderPass renderPass_FinalComposite_Window;
    VkShaderModule mainPipeline_VS;
    VkShaderModule mainPipeline_FS;
    VkDescriptorSetLayout mainPipeline_DescriptorSetLayouts[1];
    VkDescriptorSet mainPipeline_DescriptorSets[1];
    VkPipelineLayout mainPipeline_Layout;
    VkPipeline mainPipeline;
    VkSampleCountFlagBits msaaSamples;
    VkSampler sampler_Linear;
    VkSampler sampler_Nearest;
};


struct ocGraphicsImage
{
    VkImage imageVK;
    VkDeviceMemory imageMemoryVK;
    VkImageView imageViewVK;
    VkFormat format;
    VkImageUsageFlags usage;
    uint32_t sizeX;
    uint32_t sizeY;
    uint32_t mipLevels;
    VkDescriptorImageInfo descriptor;
};

struct ocGraphicsMesh
{
    ocGraphicsPrimitiveType primitiveType;
    ocGraphicsVertexFormat vertexFormat;
    VkBuffer vertexBufferVK;
    VkDeviceSize vertexBufferOffset;
    VkDeviceMemory vertexBufferMemory;
    ocGraphicsIndexFormat indexFormat;
    VkBuffer indexBufferVK;
    VkDeviceSize indexBufferOffset;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
};



///////////////////////////////////////////////////////////////////////////////
//
// GraphicsSwapchain
//
///////////////////////////////////////////////////////////////////////////////

struct ocGraphicsSwapchain : public ocGraphicsSwapchainBase
{
    VkSurfaceKHR vkSurface;
    uint32_t sizeX;
    uint32_t sizeY;
    VkSwapchainKHR vkSwapchain;
    uint32_t vkSwapchainImageCount;
    VkImage vkSwapchainImages[3];
    VkSemaphore vkNextImageSem;
    uint32_t vkCurrentImageIndex;
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

    // TODO: Optimize uniform data. Maybe use object pools as a way to group allocations?
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    VkDescriptorBufferInfo uniformBufferDescriptor;
    void* _pUniformBufferData;    // A pointer to a permanently mapped uniform buffer containing the uniform data of the object. The first bit of data is always the transform.

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

    // The list containing the current mesh objects. This is only temporary until a more efficient data structure is implemented. Needs to be a pointer
    // because we initialize to 0.
    std::vector<ocGraphicsObject*>* pObjects;


    // TEMP.
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

    uint32_t sizeX;
    uint32_t sizeY;
    glm::mat4 projection;
    glm::mat4 view;


    //// Main Framebuffer ////

    // The color buffer.
    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
    
    // The depth buffer.
    VkImage dsImage;
    VkDeviceMemory dsImageMemory;
    VkImageView dsImageView;

    // The main framebuffer for the the main rendering pass.
    VkFramebuffer mainFramebuffer;


    //// Final Composition ////
    uint32_t outputImageCount;          // For image RTs, this is always set to 1. For window RTs it's always set to the number swapchain images which will never be more than 3.
    VkImage outputImages[3];            // The images in here are created externally.
    VkImageView outputImageViews[3];
    VkFramebuffer outputFBs[3];


    //// Uniform Buffer ////
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    VkDescriptorBufferInfo uniformBufferDescriptor;
    void* pUniformBufferData;


    //// Command Buffers ////
    VkCommandBuffer cbPreTransition;    // Transitions the images of the main framebuffers to their initial states for rendering.
    VkCommandBuffer cbPreTransition_Outputs[3];
};