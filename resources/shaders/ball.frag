#version 330 core

in vec2 ex_Pos;
in vec3 ex_Color;

out vec4 out_Color;

void main(void)
{
    out_Color = vec4(ex_Color, float(ex_Pos.x * ex_Pos.x + ex_Pos.y * ex_Pos.y <= 1));
}
