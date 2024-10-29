#version 330 core

out vec4 gl_Position; 
out vec4 ex_Color;

uniform mat4 myMatrix;
uniform vec3 mouseColor;
uniform vec2 mousePosition;

void main ()
{
    gl_Position = myMatrix * vec4(mousePosition, 0.0, 1.0);
    ex_Color = vec4(mouseColor, 1.0);
}
