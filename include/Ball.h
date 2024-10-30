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

    const int 
        no_balls{ 9 }, 
        no_coords{ 3 };
        
    std::vector<Point> getCoords(const Point&);
    void LoadCenters(int);
    void CreateVBO();
    void UpdateVBO();
    GLuint CreateShaders(void);
    void DestroyVBO(void);
}
