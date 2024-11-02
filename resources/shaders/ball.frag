#version 330 core

in vec2 ex_Pos;
in vec3 ex_Color;

out vec4 out_Color;

void main(void)
{
	float p = ex_Pos.x * ex_Pos.x + ex_Pos.y * ex_Pos.y - 1;
    out_Color.xyz = ex_Color;
	// p > 0 -> opacitate 0
	out_Color.w = float(p <= 0);
	out_Color.xyz *= float(p < -0.2);
}
