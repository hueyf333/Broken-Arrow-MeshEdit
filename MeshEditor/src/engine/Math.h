// File: MeshEditor/src/engine/Math.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MeshEditor
{
    inline glm::mat4 Translate(const glm::vec3& v)
    {
        return glm::translate(glm::mat4(1.0f), v);
    }

    inline glm::mat4 Rotate(float radians, const glm::vec3& axis)
    {
        return glm::rotate(glm::mat4(1.0f), radians, axis);
    }

    inline glm::mat4 Scale(const glm::vec3& v)
    {
        return glm::scale(glm::mat4(1.0f), v);
    }
}
