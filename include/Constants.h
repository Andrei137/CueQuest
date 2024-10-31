#pragma once

#include <cmath>
#include <string>
#include <GL/glew.h>

constexpr int
    CORNERS{ 4 },
    NR_TEXTURES{ 2 },
    BALL_RADIUS{ 15 },
    NR_BALLS{ 9 },
    NR_TRIANGLE_COORDS{ 3 },
    NR_LEVELS{ 4 },
    TOTAL_BALLS{ NR_BALLS + 1 };

const float
    // AR 16:9 (factor 75 for screen, 60 for board, 8 for text)
    XMIN_SCREEN{ -600.0f },
    XMAX_SCREEN{ -XMIN_SCREEN },
    YMIN_SCREEN{ -337.5f },
    YMAX_SCREEN{ -YMIN_SCREEN },
    XMIN_BOARD{ -480.0f },
    XMAX_BOARD{ -XMIN_BOARD },
    YMIN_BOARD{ -270.5f },
    YMAX_BOARD{ -YMIN_BOARD },
    XMIN_TEXT{ -560.0f },
    XMAX_TEXT{ -432.0f },
    YMIN_TEXT{ 268.0f },
    YMAX_TEXT{ 340.0f },
    X_DISPLAY_BALL{ -410.0f },
    Y_DISPLAY_BALL{ 304.0f },
    SQRT3{ static_cast<float>(sqrt(3)) },
    PI{ static_cast<float>(atan2(-1, 0)) },
    FRICTION_MIN{ 0.94f },
    FRICTION_MAX{ 0.9999f };

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
    BALL_SHADER{ "ball" },
    TEXTURE_SHADER{ "texture" },
    LEVELS_PATH{ "resources/levels/level_" },
    SHADERS_PATH{ "resources/shaders/" },
    TEXTURES_PATH{ "resources/textures/" };
