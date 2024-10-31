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

phys::World
	scene;

phys::Circle
	pockets[6];

bool
	stationary;

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
    TextureProgramId = Texture::CreateShaders();
}

void resetPhysBalls()
{
	phys::Body* body;

	// The first NR_BALLS elements from the physics engine must be the balls, in the correct order
	for(int i{ 0 }; i < NR_BALLS; ++i)
	{
		body = scene.m_bodies[i];
		body->m_bodyData.setStatic(false, 1.f);
		// TODO: make all vectors use the same type (most likely glm::vec2)
		body->setShape(phys::Circle(phys::vec2(Ball::centers[i].x, Ball::centers[i].y), BALL_RADIUS));
		// TODO: remove this after the corner helpers work as intended
		body->m_speed.x = (float)(rand()%10*3);
		body->m_speed.y = (float)(rand()%10*3);
//		body->m_speed.x = 0.f;
//		body->m_speed.y = 0.f;
		body->m_acceleration = phys::vec2(0.f, 0.f);
	}
}

void InitPhys()
{
	// We do not allow shots to be taken in the first frame.
	stationary = false;

	// Pockets are stored separately so they don't interfere with collision checks and just exist
	int k{ 0 };
	for(float x : {XMIN_BOARD + 40.f, 0.f, XMAX_BOARD - 40.f})
		for(float y : {YMIN_BOARD + 38.f, YMAX_BOARD - 38.f})
		{
			pockets[k].m_center.x = x;
			pockets[k].m_center.y = y;
			pockets[k].setRadius(30.f);
			++k;
		}

	// Making the bodies for the balls
	for(k = 0; k < NR_BALLS; ++k)
		scene.makeBody();
	// Setting the balls' positions
	resetPhysBalls();

	phys::Body* body;

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

	// Pockets helpers
	// TODO: Fix these so the collisions are better
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

    // Physics
    InitPhys();
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
	if (!validShot)
		return;

	phys::vec2 ballCenter = static_cast<phys::Circle*>(scene.m_bodies[0]->getShape())->m_center;
	phys::vec2 mouse = phys::vec2(mousePos.x, mousePos.y);
	float len = static_cast<float>(sqrt(phys::dotProduct(mouse - ballCenter, mouse - ballCenter)));
    float strength = std::min(len - BALL_RADIUS, 200.f) * 0.25f;
    scene.m_bodies[0]->m_speed.x += (mouse.x - ballCenter.x) / len * strength;
    scene.m_bodies[0]->m_speed.y += (mouse.y - ballCenter.y) / len * strength;
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
            if (!mouseHeld && MouseInsideBall() && stationary)
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

void physEngine(int)
{
	stationary = true;

	for(int i{ 0 }; i < NR_BALLS; ++i)
	{
		phys::vec2& speed = scene.m_bodies[i]->m_speed;
		// Not quite linear interpolation
		float friction = std::max(0.f, std::min(1.f, (speed.x * speed.x + speed.y * speed.y) / 40.f)) * (FRICTION_MAX - FRICTION_MIN) + FRICTION_MIN;
		speed.x *= friction;
		speed.y *= friction;
	}

	scene.tick(1.f / TICKS_PER_SECOND, 20);

	for(int i{ 0 }; i < NR_BALLS; ++i)
	{
		if(!Ball::pocketed[i])
		{
			phys::Shape* shape = scene.m_bodies[i]->getShape();
			assert(shape->m_shapeType == phys::SH_CIRCLE);
			phys::Circle* circle = static_cast<phys::Circle*>(shape);

			for(int j{ 0 }; j < 6; ++j)
			{
				if(phys::collision::checkCollision(circle, pockets + j))
				{
					// Ball i got potted. Remove it from physics world by moving it to a coordinate outside the screen.
					// We do this so that it will not affect calculations
					scene.m_bodies[i]->m_speed = scene.m_bodies[i]->m_acceleration = phys::vec2(0.f, 0.f);
					circle->m_center.x = static_cast<float>(1e20);
					circle->m_center.y = i * 3.f /* Anything at least equal to 2 should work. */ * BALL_RADIUS;

					Ball::pocketed[i] = true;
				}
			}
		}

		auto center_i = scene.m_bodies[i]->getCenter();
		Ball::centers[i].x = center_i.x;
		Ball::centers[i].y = center_i.y;

		if(std::abs(scene.m_bodies[i]->m_speed.x) + std::abs(scene.m_bodies[i]->m_speed.x) < phys::EPS * 10.f)
		{
			scene.m_bodies[i]->m_speed.x = scene.m_bodies[i]->m_speed.y = 0.f;
		}
		else
		{
			stationary = false;
		}
	}
	Ball::UpdateVBO();

	glutPostRedisplay();
	glutTimerFunc(MSPERTICK, physEngine, 0);
}

void GenerateLevel(int newLevel=currLevel)
{
    currLevel = newLevel;
    Ball::LoadBalls(currLevel);
    resetPhysBalls();
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
void DestroyShaders()
{
    glDeleteProgram(BallProgramId);
    glDeleteProgram(TextureProgramId);
}

void Cleanup()
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
    glutTimerFunc(MSPERTICK, physEngine, 0);
    glutDisplayFunc(RenderFunction);
    glutMouseFunc(MouseClick);
    glutMotionFunc(MouseMove);
    glutPassiveMotionFunc(MouseMove);
    glutKeyboardFunc(ProcessNormalKeys);
    glutCloseFunc(Cleanup);

    glutMainLoop();

    return 0;
}
