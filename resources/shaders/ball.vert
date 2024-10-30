#version 330 core

layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec3 in_Color;
layout (location = 2) in float in_Idx;

out vec4 gl_Position;
out vec2 ex_Pos;
out vec3 ex_Color;

uniform mat4 myMatrix;

const vec2 triangleCoords[3] = vec2[3](
    vec2(0.0f, 2.0f),
    vec2(-sqrt(3), -1.0f),
    vec2(sqrt(3), -1.0f)
);

void main ()
{
    gl_Position = myMatrix * vec4(in_Position, 0.0f, 1.0f);
    ex_Pos = triangleCoords[int(in_Idx)];
    ex_Color = in_Color;
}
