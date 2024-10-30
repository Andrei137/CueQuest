#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "SOIL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "phys.h"

/* Variables Section */
GLuint
    // Board
    BoardVaoId,
    BoardVboId,
    BoardEboId,
    BoardProgramId,
    BoardMatrixLocation,
    BoardTextureLocation,
    BoardTexture,

    // Mouse
    MouseProgramId,
    MouseColorLocation,
    MousePositionLocation,
    MouseMatrixLocation;
GLfloat
    winWidth{ 1280 },
    winHeight{ 720 };
glm::vec2
    mousePos;
glm::vec3
    red{ 1.0f, 0.0f, 0.0f },
    yellow{ 1.0f, 1.0f, 0.0f };
glm::mat4
    myMatrix,
    resizeMatrix;
bool
    mousePressed{ false };
float
    xMin{ -700.f },
    xMax{ 700.f },
    yMin{ -350.f },
    yMax{ 350.f };

/* Initialization Section */
void LoadTexture(const char* photoPath, GLuint& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width{}, height{};
    unsigned char* image{ SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB) };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CreateBoardVBO(void)
{
    static const GLfloat Vertices[] =
    {
        -700.0f, -350.0f,  0.0f,  1.0f,   0.0f, 0.0f,
        700.0f, -350.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        700.0f,  350.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -700.0f,  350.0f,  0.0f,  1.0f,   0.0f, 1.0f,
    };

    static const GLuint Indices[] =
    {
        0, 1, 2, 3
    };

    glGenVertexArrays(1, &BoardVaoId);
    glBindVertexArray(BoardVaoId);

    glGenBuffers(1, &BoardVboId);
    glBindBuffer(GL_ARRAY_BUFFER, BoardVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &BoardEboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoardEboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // 0 = Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

    // 1 = Texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
}

void CreateShaders(void)
{
    // The vertex shader (.vert) affects the geonetry of the scene
    // The fragment shader (.frag) affects the color of the pixels

    BoardProgramId = LoadShaders("shaders/board_shader.vert", "shaders/board_shader.frag");
    MouseProgramId = LoadShaders("shaders/mouse_shader.vert", "shaders/mouse_shader.frag");
}

void Initialize(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    LoadTexture("textures/table.png", BoardTexture);

    CreateBoardVBO();
    CreateShaders();

    BoardMatrixLocation = glGetUniformLocation(BoardProgramId, "myMatrix");
    BoardTextureLocation = glGetUniformLocation(BoardProgramId, "myTexture");
    MouseColorLocation = glGetUniformLocation(MouseProgramId, "mouseColor");
    MousePositionLocation = glGetUniformLocation(MouseProgramId, "mousePosition");
    MouseMatrixLocation = glGetUniformLocation(MouseProgramId, "myMatrix");

    resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

/* Render Section */
void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    myMatrix = resizeMatrix;

    // Draw the board
    glUseProgram(BoardProgramId);
    glUniformMatrix4fv(BoardMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, BoardTexture);
    glUniform1i(BoardTextureLocation, 0);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));

    // Draw the mouse point
    glUseProgram(MouseProgramId);
    glUniformMatrix4fv(MouseMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
    glm::vec3 currentMouseColor = mousePressed ? red : yellow;
    glUniform3fv(MouseColorLocation, 1, &currentMouseColor[0]);
    glUniform2f(MousePositionLocation, mousePos.x, mousePos.y);
    glPointSize(20.0f);
    glDrawArrays(GL_POINTS, 4, 1);

    glutSwapBuffers();
    glFlush();
}

void MouseMove(int x, int y)
{
    float normalizedX{ x / winWidth };
    float normalizedY{ (winHeight - y) / winHeight };

    mousePos.x = normalizedX * (xMax - xMin) + xMin;
    mousePos.y = normalizedY * (yMax - yMin) + yMin;
    glutPostRedisplay();
}

void MouseClick(int button, int state, int, int)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mousePressed = true;
        }
        else if (state == GLUT_UP)
        {
            mousePressed = false;
        }
        glutPostRedisplay();
    }
}

/* Cleanup Section */
void DestroyShaders(void)
{
    glDeleteProgram(BoardProgramId);
    glDeleteProgram(MouseProgramId);
}

void DestroyVBO(void)
{
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &BoardVboId);
    glDeleteBuffers(1, &BoardEboId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &BoardVaoId);
}

void Cleanup(void)
{
    DestroyShaders();
    DestroyVBO();
}

/* Main Section */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize((int)winWidth, (int)winHeight);
    glutCreateWindow("CueQuest");

    glewInit();

    Initialize();
    glutDisplayFunc(RenderFunction);
    glutMouseFunc(MouseClick);
    glutMotionFunc(MouseMove);
    glutPassiveMotionFunc(MouseMove);
    glutCloseFunc(Cleanup);

    glutMainLoop();

    return 0;
}
