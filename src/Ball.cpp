#include "Constants.h"
#include <cmath>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include "loadShaders.h"
#include "Ball.h"

namespace Ball
{
    GLuint
        VaoId,
        VboId,
        EboId;

    std::vector<Point>
        centers;

    std::vector<bool>
        pocketed;

    bool firstInit{ true };

    std::vector<Point> GetCoords(const Point& center)
    {
        Point top{ center.x, center.y + 2 * BALL_RADIUS };
        Point left_bottom{ center.x - SQRT3 * BALL_RADIUS, center.y - BALL_RADIUS };
        Point right_bottom{ center.x + SQRT3 * BALL_RADIUS, center.y - BALL_RADIUS };
        return {
            top,
            left_bottom,
            right_bottom
        };
    }

    void LoadBalls(int currLevel)
    {
        centers.clear();
        pocketed = std::vector<bool>(NR_BALLS, false);
        std::ifstream file(LEVELS_PATH + std::to_string(currLevel) + ".txt");
        Point center;
        while (file >> center)
        {
            centers.push_back(center);
        }
        file.close();
    }

    int GetCurrentBall()
    {
        for (int i = 1; i < NR_BALLS; ++i)
        {
            if (!pocketed[i])
            {
                return i;
            }
        }
        return 0;
    }

    void UpdateVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VboId);

        const std::vector<glm::vec3> colors{
            { 1.000f, 1.000f, 1.000f }, // 0 - white
            { 0.998f, 0.866f, 0.007f }, // 1 - yellow
            { 0.196f, 0.525f, 0.988f }, // 2 - blue
            { 0.796f, 0.003f, 0.098f }, // 3 - red
            { 0.470f, 0.007f, 0.850f }, // 4 - purple
            { 0.956f, 0.541f, 0.113f }, // 5 - orange
            { 0.160f, 0.796f, 0.180f }, // 6 - green
            { 0.368f, 0.003f, 0.082f }, // 7 - brown
            { 0.058f, 0.050f, 0.039f }  // 8 - black
        };

        // The last ball is the one showing which ball needs to be hit
        static GLfloat Vertices[6 * NR_TRIANGLE_COORDS * TOTAL_BALLS];
        int currIdx{ 0 };

        // Handle the playable balls
        for (int i = 0; i < NR_BALLS; ++i)
        {
            std::vector<Point> coords{ GetCoords(centers[i]) };

            for (int j = 0; j < NR_TRIANGLE_COORDS; ++j)
            {
                // Position
                if (pocketed[i])
                {
                    Vertices[currIdx++] = Vertices[currIdx++] = 0.0f;
                }
                else
                {
                    Vertices[currIdx++] = coords[j].x;
                    Vertices[currIdx++] = coords[j].y;
                }

                if (firstInit)
                {
                    // Color
                    Vertices[currIdx++] = colors[i].r;
                    Vertices[currIdx++] = colors[i].g;
                    Vertices[currIdx++] = colors[i].b;

                    // Triangle Position Index
                    Vertices[currIdx++] = (float)j;
                }
                else
                {
                    currIdx += 4;
                }
            }
        }

        // Handle the display ball
        std::vector<Point> coords(3);
        if (firstInit)
        {
            coords = GetCoords(Point{ X_DISPLAY_BALL, Y_DISPLAY_BALL });
        }
        for (int j = 0; j < NR_TRIANGLE_COORDS; ++j)
        {
            if (firstInit)
            {
                // Position
                Vertices[currIdx++] = coords[j].x;
                Vertices[currIdx++] = coords[j].y;
            }
            else
            {
                currIdx += 2;
            }

            // Color (the same as the current ball)
            Vertices[currIdx++] = colors[GetCurrentBall()].r;
            Vertices[currIdx++] = colors[GetCurrentBall()].g;
            Vertices[currIdx++] = colors[GetCurrentBall()].b;

            if (firstInit)
            {
                // Triangle Position Index
                Vertices[currIdx++] = (float)j;
            }
            else
            {
                currIdx += 1;
            }
        }

        firstInit = false;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    }

    void CreateVBO()
    {
        // The last ball is the one showing which ball needs to be hit
        static GLuint Indices[3 * TOTAL_BALLS];
        for (int i = 0; i < 3 * TOTAL_BALLS; ++i)
        {
            Indices[i] = i;
        }

        glGenVertexArrays(1, &VaoId);
        glBindVertexArray(VaoId);

        glGenBuffers(1, &VboId);
        glBindBuffer(GL_ARRAY_BUFFER, VboId);
        glBufferData(GL_ARRAY_BUFFER, 6 * NR_TRIANGLE_COORDS * TOTAL_BALLS * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

        UpdateVBO();

        glGenBuffers(1, &EboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

        // 0 = Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

        // 1 = Color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

        // 2 = Triangle's Position Index
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    }

    GLuint CreateShaders()
    {
        return LoadShaders(
            (SHADERS_PATH + BALL_SHADER + ".vert").c_str(),
            (SHADERS_PATH + BALL_SHADER + ".frag").c_str()
        );
    }

    void DestroyVBO()
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

    int pot(int a_ballId)
    {
    	pocketed[a_ballId] = true;

    	if(a_ballId)
			return -1;

    	int inversions{ 0 };

    	for(int i{ 1 }; i < a_ballId; ++i)
			inversions += !pocketed[i];

		return inversions;
    }
}
