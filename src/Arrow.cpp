#include "Constants.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "Ball.h"
#include "Arrow.h"

namespace Arrow
{
    GLuint
        VaoId,
        VboId,
        EboId;

    void UpdateVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VboId);

        static GLfloat Vertices[8];
        int currIdx{ 0 };

        // The center of the white ball
        Vertices[0] = Ball::centers[0].x;
        Vertices[1] = Ball::centers[0].y;

        // Opposite side of the mouse
        Vertices[2] = 10.0f;
        Vertices[3] = 10.0f;

        // Arrow point 1
        Vertices[4] = 20.0f;
        Vertices[5] = 20.0f;

        // Arrow point 2
        Vertices[6] = 30.0f;
        Vertices[7] = 30.0f;

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    }

    void CreateVBO()
    {
        static const GLuint Indices[] =
        {
            0, 1,
            2, 1,
            3, 1
        };

        glGenVertexArrays(1, &VaoId);
        glBindVertexArray(VaoId);

        glGenBuffers(1, &VboId);
        glBindBuffer(GL_ARRAY_BUFFER, VboId);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

        UpdateVBO();

        glGenBuffers(1, &EboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

        // 0 = Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    }

    GLuint CreateShaders()
    {
        return LoadShaders(
            (SHADERS_PATH + ARROW_SHADER + ".vert").c_str(),
            (SHADERS_PATH + ARROW_SHADER + ".frag").c_str()
        );
    }

    void DestroyVBO()
    {
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VboId);
        glDeleteBuffers(1, &EboId);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &VaoId);
    }
}
