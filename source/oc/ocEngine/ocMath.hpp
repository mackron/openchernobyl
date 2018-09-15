// Copyright (C) 2018 David Reid. See included LICENSE file.

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

OC_INLINE glm::vec3 ocMakeAbsolutePosition(const glm::vec3 &relativePosition, const glm::vec3 &relativeTo)
{
    return relativeTo + relativePosition;
}

OC_INLINE glm::vec3 ocMakeRelativePosition(const glm::vec3 &absolutePosition, const glm::vec3 &relativeTo)
{
    return absolutePosition - relativeTo;
}

OC_INLINE glm::quat ocMakeAbsoluteRotation(const glm::quat &relativeRotation, const glm::quat &relativeTo)
{
    return relativeTo * relativeRotation;
}

OC_INLINE glm::quat ocMakeRelativeRotation(const glm::quat &absoluteRotation, const glm::quat &relativeTo)
{
    return glm::inverse(relativeTo) * absoluteRotation;
}

OC_INLINE glm::vec3 ocMakeAbsoluteScale(const glm::vec3 &relativeScale, const glm::vec3 &relativeTo)
{
    return relativeTo * relativeScale;
}

OC_INLINE glm::vec3 ocMakeRelativeScale(const glm::vec3 &absoluteScale, const glm::vec3 &relativeTo)
{
    return absoluteScale / relativeTo;
}
