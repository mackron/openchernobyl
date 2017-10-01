//
// Vertex
//
#if defined(SHADER_STAGE_VERTEX)
    #ifdef VULKAN
        layout(location = 0) in vec3 VERT_Position;
        layout(location = 1) in vec2 VERT_TexCoord;
        layout(location = 2) in vec3 VERT_Normal;
        #ifdef OC_VERTEX_FORMAT_V3T2N3T3B3
            layout(location = 3) in vec3 VERT_Tangent;
            layout(location = 4) in vec3 VERT_Binormal;
        #endif
        
        layout(location = 0) out vec2 FRAG_TexCoord;
        layout(location = 1) out vec3 FRAG_Normal;
        #ifdef OC_VERTEX_FORMAT_V3T2N3T3B3
            layout(location = 2) out vec3 FRAG_Tangent;
            layout(location = 3) out vec3 FRAG_Binormal;
        #endif
        
        layout (set = 0, binding = 0) uniform UBO_Camera
        {
        	mat4 Projection;
        	mat4 View;
        } Camera;
        
        layout (set = 0, binding = 1) uniform UBO_Object
        {
            mat4 Model;
        } Object;
    #else
        attribute vec3 VERT_Position;
        attribute vec2 VERT_TexCoord;
        attribute vec3 VERT_Normal;
        #ifdef OC_VERTEX_FORMAT_V3T2N3T3B3
            attribute vec3 VERT_Tangent;
            attribute vec3 VERT_Binormal;
        #endif
        
        varying vec2 FRAG_TexCoord;
        varying vec3 FRAG_Normal;
        #ifdef OC_VERTEX_FORMAT_V3T2N3T3B3
            varying vec3 FRAG_Tangent;
            varying vec3 FRAG_Binormal;
        #endif
        
        struct UBO_Camera
        {
            mat4 Projection;
            mat4 View;
        };
        uniform UBO_Camera Camera;
        
        struct UBO_Object
        {
            mat4 Model;
        };
        uniform UBO_Object Object;
    #endif
#endif


//
// Fragment
//
#if defined(SHADER_STAGE_FRAGMENT)
    #ifdef VULKAN
        // Inputs
        layout (location = 0) in vec2 FRAG_TexCoord;
        layout (location = 1) in vec3 FRAG_Normal;
        #ifdef OC_VERTEX_FORMAT_V3T2N3T3B3
            layout (location = 2) in vec2 FRAG_Tangent;
            layout (location = 3) in vec3 FRAG_Binormal;
        #endif
        
        // Outputs
        layout (location = 0) out vec4 OUT_Color;
        
        // Uniforms
        layout (binding = 2) uniform sampler2D Texture0;
    #else
        // Inputs
        varying vec2 FRAG_TexCoord;
        varying vec3 FRAG_Normal;
        #ifdef OC_VERTEX_FORMAT_V3T2N3T3B3
            varying vec2 FRAG_Tangent;
            varying vec3 FRAG_Binormal;
        #endif
        
        // Outputs
        vec4 OUT_Color;
        
        // Uniforms
        uniform sampler2D Texture0;
    #endif
#endif
