#pragma once

namespace Ball
{
    extern GLuint
        VaoId, 
        VboId, 
        EboId;

    void CreateVBO(void);
    GLuint CreateShaders(void);
    void DestroyVBO(void);
}

