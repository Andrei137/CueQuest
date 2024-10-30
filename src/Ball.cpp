#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "Ball.h"

namespace Ball
{
    GLuint
        VaoId,
        VboId,
        EboId;

    void CreateVBO(void)
    {
        const float sqrt3{ static_cast<float>(sqrt(3)) };

        static const GLfloat Vertices[] =
        {
            0.0f, 50.0f,   1.0f, 0.0f, 0.0f,  0.0f, 2.0f,
            -sqrt3 * 25.0f, -25.0f,   1.0f, 0.0f, 0.0f,   -sqrt3, -1.0f,
            sqrt3 * 25.0f, -25.0f,   1.0f, 0.0f, 0.0f,   sqrt3, -1.0f
        };

        static const GLuint Indices[] =
        {
            0, 1, 2
        };

        glGenVertexArrays(1, &VaoId);
        glBindVertexArray(VaoId);

        glGenBuffers(1, &VboId);
        glBindBuffer(GL_ARRAY_BUFFER, VboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
        
        glGenBuffers(2, &EboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

        // 0 = Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
        
        // 1 = Color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

        // 2 = Triangle's Position
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    }

    GLuint CreateShaders(void)
    {
        return LoadShaders("shaders/circle.vert", "shaders/circle.frag");
    }

    void DestroyVBO(void)
    {
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VboId);
        glDeleteBuffers(1, &EboId);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &VaoId);
    }
}
