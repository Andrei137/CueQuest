﻿#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h> 
#include <GL/freeglut.h>
#include "SOIL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "phys.h"
#include "Board.h"
#include "Ball.h"

/* Variables Section */
GLuint
    // Ball
    BallProgramId,
    BallMatrixLocation,

    // Board
    BoardProgramId,
    BoardMatrixLocation,
    BoardTextureLocation,
    BoardTexture;
const GLfloat 
    winWidth{ 1280 }, 
    winHeight{ 720 };
glm::vec2 
    mousePos;
glm::mat4 
    myMatrix, 
    resizeMatrix;
bool 
    mousePressed{ false };
const float 
    // AR 16:9 (factor 75)
    xMin{ -600.0f },
    xMax{ 600.0f }, 
    yMin{ -337.5f }, 
    yMax{ 337.5f };

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

void CreateShaders(void)
{
    // The vertex shader (.vert) affects the geonetry of the scene
    // The fragment shader (.frag) affects the color of the pixels

    BallProgramId = Ball::CreateShaders();
    BoardProgramId = Board::CreateShaders();
}

void Initialize(void)
{
    glClearColor(0.10f, 0.16f, 0.25f, 1.0f);

    LoadTexture("textures/table.png", BoardTexture);

    CreateShaders();

    // Ball
    Ball::CreateVBO();
    BallMatrixLocation = glGetUniformLocation(BallProgramId, "myMatrix");

    // Board
    Board::CreateVBO();
    BoardMatrixLocation = glGetUniformLocation(BoardProgramId, "myMatrix");
    BoardTextureLocation = glGetUniformLocation(BoardProgramId, "myTexture");

    // Transformations
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
    glBindVertexArray(Board::VaoId);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));
     
    // Draw the ball
    glUseProgram(BallProgramId);
    glUniformMatrix4fv(BallMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
    glBindVertexArray(Ball::VaoId);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(0));

    glutSwapBuffers();
    glFlush();
}

void MouseMove(int x, int y)
{
    const float normalizedX{ x / winWidth };
    const float normalizedY{ (winHeight - y) / winHeight };

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
    glDeleteProgram(BallProgramId);
    glDeleteProgram(BoardProgramId);
}

void Cleanup(void)
{
    DestroyShaders();
    Ball::DestroyVBO();
    Board::DestroyVBO();
}

/* Main Section */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("CueQuest");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

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
