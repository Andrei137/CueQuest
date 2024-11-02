#version 330 core

layout (location = 0) in vec2 in_Position;

out vec4 gl_Position; 
out vec4 ex_Color;

uniform mat4 myMatrix;
uniform vec3 arrowColor;

void main ()
{
    gl_Position = myMatrix * vec4(in_Position, 0.0f, 1.0f);
    ex_Color.xyz = arrowColor;
	ex_Color.w = 1.f;
}
