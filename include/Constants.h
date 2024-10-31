#pragma once

#include <cmath>
#include <string>
#include <GL/glew.h>

constexpr int
    BALL_RADIUS{ 15 },
    NO_BALLS{ 9 },
    NO_TRIANGLE_COORDS{ 3 };

const float
    // AR 16:9 (factor 75 for screen, 60 for board)
    XMIN_SCREEN{ -600.0f },
    XMAX_SCREEN{ -XMIN_SCREEN },
    YMIN_SCREEN{ -337.5f },
    YMAX_SCREEN{ -YMIN_SCREEN },
    XMIN_BOARD{ -480.0f },
    XMAX_BOARD{ -XMIN_BOARD },
    YMIN_BOARD{ -270.5f },
    YMAX_BOARD{ -YMIN_BOARD },
    SQRT3{ static_cast<float>(sqrt(3)) },
    PI{ static_cast<float>(atan2(-1, 0)) };

const GLuint
	TICKS_PER_SECOND{ 30 },
	MSPERTICK{ 1000 / TICKS_PER_SECOND };

const GLint
    POSX{ 350 },
    POSY{ 200 };

const GLfloat
    WIDTH{ 1280 },
    HEIGHT{ 720 };

const std::string
    TITLE{ "CueQuest" },
    LEVELS_PATH{ "resources/levels/level_" },
    SHADERS_PATH{ "resources/shaders/" },
    TEXTURES_PATH{ "resources/textures/" };
