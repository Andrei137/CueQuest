#pragma once

namespace Arrow
{
    extern GLuint
        VaoId,
        VboId,
        EboId;

    void CreateVBO();
    void UpdateVBO();
    GLuint CreateShaders();
    void DestroyVBO();
}
