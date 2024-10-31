#pragma once

namespace Texture
{
    extern GLuint
        VaoId,
        VboId,
        EboId;

    void CreateVBO();
    GLuint CreateShaders();
    void DestroyVBO();
}
