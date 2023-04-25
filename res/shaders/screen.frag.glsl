#version 330 core

out vec4 frag;
in vec2 texCoords;

uniform sampler2D tex;

void main()
{
    frag = vec4(texture(tex, texCoords).rgb, 1.0);
}