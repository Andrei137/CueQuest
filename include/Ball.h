#pragma once

#include <iostream>
#include <vector>

namespace Ball
{
    struct Point
    {
        float x{}, y{};

        Point& operator= (const Point& a_P)
        {
            this->x = a_P.x;
            this->y = a_P.y;
            return *this;
        }

        friend std::istream& operator>>(std::istream& a_in, Point& a_p)
        {
            return a_in >> a_p.x >> a_p.y;
        }
    };

    extern GLuint
        VaoId,
        VboId,
        EboId;
        
    extern Point
        whiteBallCenter;

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
    int pot(int);
}
