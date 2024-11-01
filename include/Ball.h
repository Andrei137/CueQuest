#pragma once

#include <iostream>
#include <vector>

namespace Ball
{
    struct Point
    {
        float x{}, y{};

        friend std::istream& operator>>(std::istream& is, Point& p)
        {
            return is >> p.x >> p.y;
        }
    };

    extern GLuint
        VaoId,
        VboId,
        EboId;

    extern std::vector<Point>
        centers;

    extern std::vector<bool>
        pocketed;

    std::vector<Point> GetCoords(const Point&);
    void LoadBalls(int);
    int GetCurrentBall();
    void CreateVBO();
    void UpdateVBO();
    GLuint CreateShaders();
    void DestroyVBO();
    /// Pots the ball in a pocket. Returns the inversions caused by this or -1 if the cue ball was potted
    int pot(int);
}
