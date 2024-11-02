#include <algorithm>
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

    void UpdateVBO(glm::vec2 a_cueBallPos, glm::vec2 a_mousePos)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VboId);

        static GLfloat Vertices[8];
        int currIdx{ 0 };
        glm::vec2 normal = a_cueBallPos - a_mousePos;
        float magnitude{ static_cast<float>(sqrt(glm::dot(normal, normal))) };
        normal.x /= magnitude;
        normal.y /= magnitude;
        float arrowMagnitude{ std::min(magnitude, MAX_STRENGTH) };

        // The center of the white ball
        Vertices[0] = a_cueBallPos.x;
        Vertices[1] = a_cueBallPos.y;

        // Opposite side of the mouse
        Vertices[2] = a_cueBallPos.x + normal.x * arrowMagnitude;
        Vertices[3] = a_cueBallPos.y + normal.y * arrowMagnitude;

        // Arrow point 1
        Vertices[4] = a_cueBallPos.x + normal.x * arrowMagnitude - SQRT2 * 0.5f * ARROW_SIDE_LENGTH * (normal.x - normal.y) * arrowMagnitude / MAX_STRENGTH;
        Vertices[5] = a_cueBallPos.y + normal.y * arrowMagnitude - SQRT2 * 0.5f * ARROW_SIDE_LENGTH * (normal.x + normal.y) * arrowMagnitude / MAX_STRENGTH;

        // Arrow point 2
        Vertices[6] = a_cueBallPos.x + normal.x * arrowMagnitude - SQRT2 * 0.5f * ARROW_SIDE_LENGTH * (normal.x + normal.y) * arrowMagnitude / MAX_STRENGTH;
        Vertices[7] = a_cueBallPos.y + normal.y * arrowMagnitude - SQRT2 * 0.5f * ARROW_SIDE_LENGTH * (-normal.x + normal.y) * arrowMagnitude / MAX_STRENGTH;

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

        UpdateVBO(glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f));

        glGenBuffers(1, &EboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

        // 0 = Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
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
