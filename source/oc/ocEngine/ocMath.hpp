// Copyright (C) 2017 David Reid. See included LICENSE file.

OC_INLINE glm::mat4 ocMakeMat4(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
{
    // rotation.
    glm::mat4 result = glm::mat4_cast(rotation);

    // position.
    result[3][0] = position.x;
    result[3][1] = position.y;
    result[3][2] = position.z;

    // scale.
    result[0] *= scale.x;
    result[1] *= scale.y;
    result[2] *= scale.z;

    return result;
}

OC_INLINE glm::mat4 ocMakeMat4(const glm::vec4 &position, const glm::quat &rotation, const glm::vec4 &scale)
{
    return ocMakeMat4(glm::vec3(position), rotation, glm::vec3(scale));
}

OC_INLINE glm::mat4 ocMakeMat4_VulkanClipCorrection()
{
    glm::mat4 result;
    result[0] = glm::vec4(1,  0, 0,    0);
    result[1] = glm::vec4(0, -1, 0,    0);
    result[2] = glm::vec4(0,  0, 0.5f, 0);
    result[3] = glm::vec4(0,  0, 0.5f, 1);
    return result;
}