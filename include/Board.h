#pragma once

namespace Board
{
    extern GLuint
        VaoId,
        VboId,
        EboId;

    void CreateVBO();
    GLuint CreateShaders();
    void DestroyVBO();
}
