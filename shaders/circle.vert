#version 330 core

layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec3 in_Color;
layout (location = 2) in vec2 in_LocalPosition;

out vec4 gl_Position;
out vec2 ex_Pos;
out vec3 ex_Color;

uniform mat4 myMatrix;

void main ()
{
    gl_Position = myMatrix * vec4(in_Position, 0.0f, 1.0f);
    ex_Pos = in_LocalPosition;
    ex_Color = in_Color;
}
