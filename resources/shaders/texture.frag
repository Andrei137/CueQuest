#version 330 core

in vec2 tex_Coord;

out vec4 out_Color;

uniform sampler2D myTexture;

void main(void)
{
    out_Color = texture(myTexture, tex_Coord);
}