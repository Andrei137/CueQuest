#pragma once
#include "glm/glm.hpp"

namespace Arrow
{
    extern GLuint
        VaoId,
        VboId,
        EboId;

    void CreateVBO();
    void UpdateVBO(glm::vec2 a_cueBallPos, glm::vec2 a_mousePos);
    GLuint CreateShaders();
    void DestroyVBO();
}
