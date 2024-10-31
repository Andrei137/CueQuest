#pragma once

namespace Texture
{
    extern GLuint
        VaoId, 
        VboId, 
        EboId;

    void CreateVBO(void);
    GLuint CreateShaders(void);
    void DestroyVBO(void);
}
