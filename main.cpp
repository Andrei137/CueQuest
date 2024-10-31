#include "Constants.h"
#include <windows.h>
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
#include "Ball.h"
#include "Texture.h"

/* Variables Section */
GLuint
    // Ball
    BallProgramId,
    BallMatrixLocation,

    // Board & Text
    TextureProgramId,
    TextureMatrixLocation,
    TextureLocation,
    BoardTexture,
    TextTexture;
glm::vec2 
    mousePos;
glm::mat4 
    myMatrix, 
    resizeMatrix;
bool
    mouseHeld{ false },
    mouseShoot{ false };
int
    currLevel{ 1 };

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
    BallProgramId = Ball::CreateShaders();
    TextureProgramId = Texture::CreateShaders();
}

void Initialize(void)
{
    glClearColor(0.0784f, 0.1372f, 0.3019f, 1.0f);

    LoadTexture((TEXTURES_PATH + "board.png").c_str(), BoardTexture);
    LoadTexture((TEXTURES_PATH + "text.png").c_str(), TextTexture);

    CreateShaders();

    // Ball
    Ball::LoadBalls(currLevel);
    Ball::CreateVBO();
    BallMatrixLocation = glGetUniformLocation(BallProgramId, "myMatrix");

    // Board & Text
    Texture::CreateVBO();
    TextureMatrixLocation = glGetUniformLocation(TextureProgramId, "myMatrix");
    TextureLocation = glGetUniformLocation(TextureProgramId, "myTexture");

    // Transformations
    resizeMatrix = glm::ortho(XMIN_SCREEN, XMAX_SCREEN, YMIN_SCREEN, YMAX_SCREEN);
}

/* Render Section */
bool MouseInsideBall()
{
    const float dx{ mousePos.x - Ball::centers[0].x },
                dy{ mousePos.y - Ball::centers[0].y },
                distanceSqared{ dx * dx + dy * dy };
    return distanceSqared <= BALL_RADIUS * BALL_RADIUS;
}

void HandleShoot(bool validShot)
{
    Ball::centers[0] = Ball::Point{ 
        (rand() % (int)(XMAX_BOARD - XMIN_BOARD)) + XMIN_BOARD, 
        (rand() % (int)(YMAX_BOARD - YMIN_BOARD)) + YMIN_BOARD 
    };
    Ball::UpdateVBO();
}

void MouseMove(int x, int y)
{
    const float normalizedX{ x / WIDTH },
                normalizedY{ (HEIGHT - y) / HEIGHT };

    mousePos.x = normalizedX * (XMAX_SCREEN - XMIN_SCREEN) + XMIN_SCREEN;
    mousePos.y = normalizedY * (YMAX_SCREEN - YMIN_SCREEN) + YMIN_SCREEN;

    glutPostRedisplay();
}

void MouseClick(int button, int state, int, int)
{
    if (button == GLUT_LEFT_BUTTON) 
    {
        if (state == GLUT_DOWN)
        {
            if (!mouseHeld && MouseInsideBall())
            {
                mouseHeld = true;
            }
        }
        else if (state == GLUT_UP)
        {
            if (mouseHeld)
            {
                mouseHeld = false;
                HandleShoot(!MouseInsideBall());
            }
        }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN) 
        {
            if (XMIN_BOARD < mousePos.x && mousePos.x < XMAX_BOARD &&
                YMIN_BOARD < mousePos.y && mousePos.y < YMAX_BOARD)
            {
                // Ball::centers[0] = Ball::Point{ mousePos.x, mousePos.y };
                int currBall{ Ball::GetCurrentBall() };
                if (currBall != 0)
                {
                    Ball::pocketed[Ball::GetCurrentBall()] = true;
                    Ball::UpdateVBO();
                }
            }
        }
    }
    glutPostRedisplay();
}

void GenerateLevel(int newLevel=currLevel)
{
    currLevel = newLevel;
    Ball::LoadBalls(currLevel);
    Ball::UpdateVBO();
    glutPostRedisplay();
}

void ProcessNormalKeys(unsigned char key, int x, int y)
{
    if (key == 'r')
    {
        GenerateLevel();
    }
    else if (key >= '1' && key <= '0' + NR_LEVELS)
    {
        GenerateLevel(key - '0');
    }
}

void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    myMatrix = resizeMatrix;

    // Draw the board
    glUseProgram(TextureProgramId);   
    glUniformMatrix4fv(TextureMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, BoardTexture);
    glUniform1i(TextureLocation, 0);
    glBindVertexArray(Texture::VaoId);
    glDrawElements(GL_POLYGON, CORNERS, GL_UNSIGNED_INT, (void*)(0));

    // Draw the text
    glBindTexture(GL_TEXTURE_2D, TextTexture);
    glDrawElements(GL_POLYGON, CORNERS, GL_UNSIGNED_INT, (void*)(4 * sizeof(GLuint)));
     
    // Draw the balls
    glUseProgram(BallProgramId);
    glUniformMatrix4fv(BallMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
    glBindVertexArray(Ball::VaoId);
    glDrawElements(GL_TRIANGLES, NR_TRIANGLE_COORDS * TOTAL_BALLS, GL_UNSIGNED_INT, (void*)(0));

    glutSwapBuffers();
    glFlush();
}

/* Cleanup Section */
void DestroyShaders(void)
{
    glDeleteProgram(BallProgramId);
    glDeleteProgram(TextureProgramId);
}

void Cleanup(void)
{
    DestroyShaders();
    Ball::DestroyVBO();
    Texture::DestroyVBO();
}

/* Main Section */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(POSX, POSY);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow(TITLE.c_str());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    glewInit();

    Initialize();
    glutDisplayFunc(RenderFunction);
    glutMouseFunc(MouseClick);
    glutMotionFunc(MouseMove);
    glutPassiveMotionFunc(MouseMove);
    glutKeyboardFunc(ProcessNormalKeys);
    glutCloseFunc(Cleanup);

    glutMainLoop();

    return 0;
}
