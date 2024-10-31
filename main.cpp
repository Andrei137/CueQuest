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
glm::vec2
    mousePos;
glm::mat4
    myMatrix,
    resizeMatrix;
bool
    mousePressed{ false };
int
    currLevel{ 1 };

phys::World
	scene;

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

void CreateShaders()
{
    BallProgramId = Ball::CreateShaders();
    BoardProgramId = Board::CreateShaders();
}

void InitPhys()
{
	phys::Body* body;

	rand();
	// Primele NO_BALLS elemente din phys engine trebuie sa fie bilele in ordinea corecta
	for(int i{ 0 }; i < NO_BALLS; ++i)
	{
		body = scene.makeBody();
		body->m_bodyData.setStatic(false, 1.f);
		// TODO: make all vectors use the same type (most likely glm::vec2)
		body->setShape(phys::Circle(phys::vec2(Ball::centers[i].x, Ball::centers[i].y), BALL_RADIUS));
		// TODO: remove this after the test
		body->m_speed.x = rand()%10;
		body->m_speed.y = rand()%10;
//		}
	}

	// Left
	body = scene.makeBody();
	body->setShape(phys::AxisParalelRectangle(phys::vec2(XMIN_BOARD, 0.f), phys::vec2(55.f, YMAX_BOARD - 110)));

	// Right
	body = scene.makeBody();
	body->setShape(phys::AxisParalelRectangle(phys::vec2(XMAX_BOARD, 0.f), phys::vec2(55.f, YMAX_BOARD - 110)));

	// Top left
	body = scene.makeBody();
	body->setShape(phys::AxisParalelRectangle(phys::vec2(-210.f, YMIN_BOARD), phys::vec2(172.5f, 60.f)));

	// Top right
	body = scene.makeBody();
	body->setShape(phys::AxisParalelRectangle(phys::vec2( 210.f, YMIN_BOARD), phys::vec2(172.5f, 60.f)));

	// Bottom left
	body = scene.makeBody();
	body->setShape(phys::AxisParalelRectangle(phys::vec2(-210.f, YMAX_BOARD), phys::vec2(172.5f, 60.f)));

	// Bottom right
	body = scene.makeBody();
	body->setShape(phys::AxisParalelRectangle(phys::vec2( 210.f, YMAX_BOARD), phys::vec2(172.5f, 60.f)));

	// Pockets
	// Top right
	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(453.5f, 166.5f), phys::vec2(25.f, 25.f), PI / 3.f));

	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(384.f, 243.5f), phys::vec2(25.f, 25.f), -2.f * PI / 9.f));

	// Bottom right
	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(453.5f, -166.5f), phys::vec2(25.f, 25.f), -PI / 3.f));

	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(384.f, -243.5f), phys::vec2(25.f, 25.f), 2.f * PI / 9.f));

	// Top left
	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(-453.5f, 166.5f), phys::vec2(25.f, 25.f), 2.f * PI / 3.f));

	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(-384.f, 243.5f), phys::vec2(25.f, 25.f), 11.f * PI / 9.f));

	// Bottom left
	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(-453.5f, -166.5f), phys::vec2(25.f, 25.f), 4.f * PI / 3.f));

	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(-384.f, -243.5f), phys::vec2(25.f, 25.f), 7.f * PI / 9.f));

	// Top middle
	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(41.5f, 225.f), phys::vec2(12.5f, 12.5f), 23.f * PI / 180.f));

	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(-41.5f, 225.f), phys::vec2(12.5f, 12.5f), 157.f * PI / 180.f));

	// Bottom middle
	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(41.5f, -225.f), phys::vec2(12.5f, 12.5f), -23.f * PI / 180.f));

	body = scene.makeBody();
	body->setShape(phys::RotatibleRectangle(phys::vec2(-41.5f, -225.f), phys::vec2(12.5f, 12.5f), 203.f * PI / 180.f));
}

void Initialize()
{
    glClearColor(0.10f, 0.16f, 0.25f, 1.0f);

    LoadTexture((TEXTURES_PATH + "board.png").c_str(), BoardTexture);

    CreateShaders();

    // Ball
    Ball::LoadBalls(currLevel);
    Ball::CreateVBO();
    BallMatrixLocation = glGetUniformLocation(BallProgramId, "myMatrix");

    // Board
    Board::CreateVBO();
    BoardMatrixLocation = glGetUniformLocation(BoardProgramId, "myMatrix");
    BoardTextureLocation = glGetUniformLocation(BoardProgramId, "myTexture");

    // Transformations
    resizeMatrix = glm::ortho(XMIN_SCREEN, XMAX_SCREEN, YMIN_SCREEN, YMAX_SCREEN);

    // Physics
    InitPhys();
}

/* Render Section */
void MouseMove(int x, int y)
{
    const float normalizedX{ x / WIDTH };
    const float normalizedY{ (HEIGHT - y) / HEIGHT };

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
            mousePressed = true;

            if (XMIN_BOARD < mousePos.x && mousePos.x < XMAX_BOARD &&
                YMIN_BOARD < mousePos.y && mousePos.y < YMAX_BOARD)
            {
                Ball::centers[0] = Ball::Point{ mousePos.x, mousePos.y };
                Ball::UpdateVBO();
            }

        }
        else if (state == GLUT_UP)
        {
            mousePressed = false;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_UP)
        {
            if (currLevel == 1)
            {
                ++currLevel;
            }
            else
            {
                --currLevel;
            }
            Ball::LoadBalls(currLevel);
            Ball::UpdateVBO();
        }

    }
    glutPostRedisplay();
}

void physEngine(int)
{
	scene.tick(1.f / TICKS_PER_SECOND, 20);

	for(int i{ 0 }; i < NO_BALLS; ++i)
	{
		auto center_i = scene.m_bodies[i]->getCenter();
		Ball::centers[i].x = center_i.x;
		Ball::centers[i].y = center_i.y;
	}
	Ball::UpdateVBO();

	glutPostRedisplay();
	glutTimerFunc(MSPERTICK, physEngine, 0);
}

void RenderFunction()
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

    // Draw the balls
    glUseProgram(BallProgramId);
    glUniformMatrix4fv(BallMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
    glBindVertexArray(Ball::VaoId);
    glDrawElements(GL_TRIANGLES, NO_BALLS * NO_TRIANGLE_COORDS, GL_UNSIGNED_INT, (void*)(0));

    glutSwapBuffers();
    glFlush();
}

/* Cleanup Section */
void DestroyShaders()
{
    glDeleteProgram(BallProgramId);
    glDeleteProgram(BoardProgramId);
}

void Cleanup()
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
    glutInitWindowPosition(POSX, POSY);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow(TITLE.c_str());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    glewInit();

    Initialize();
    glutTimerFunc(MSPERTICK, physEngine, 0);
    glutDisplayFunc(RenderFunction);
    glutMouseFunc(MouseClick);
    glutMotionFunc(MouseMove);
    glutPassiveMotionFunc(MouseMove);
    glutCloseFunc(Cleanup);

    glutMainLoop();

    return 0;
}
