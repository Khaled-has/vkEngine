#pragma once

#include <iostream>

#include <glm/glm.hpp>

namespace Engine
{

    struct TagComponent
    {
        std::string mTag;
    };

    struct TransformComponent
    {
        glm::vec3 mPosition;
        glm::vec3 mScale;
        glm::vec3 mRotation;
    };

}