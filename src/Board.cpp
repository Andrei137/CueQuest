#include "Constants.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"

namespace Board
{
    GLuint
        VaoId, 
        VboId, 
        EboId;

    void CreateVBO(void)
    {
        // AR 16:9 (factor 60)
        static const GLfloat Vertices[] =
        {
            XMIN_BOARD, YMIN_BOARD, 0.0f, 0.0f,
            XMAX_BOARD, YMIN_BOARD, 1.0f, 0.0f,
            XMAX_BOARD,  YMAX_BOARD, 1.0f, 1.0f,
            XMIN_BOARD,  YMAX_BOARD, 0.0f, 1.0f,
        };

        static const GLuint Indices[] =
        {
            0, 1, 2, 3
        };

        glGenVertexArrays(1, &VaoId);
        glBindVertexArray(VaoId);

        glGenBuffers(1, &VboId);
        glBindBuffer(GL_ARRAY_BUFFER, VboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
        
        glGenBuffers(1, &EboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

        // 0 = Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        
        // 1 = Texture
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    }

    GLuint CreateShaders(void)
    {
        return LoadShaders(
            (SHADERS_PATH + "board.vert").c_str(),
            (SHADERS_PATH + "board.frag").c_str()
        );
    }

    void DestroyVBO(void)
    {
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VboId);
        glDeleteBuffers(1, &EboId);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &VaoId);
    }
}
