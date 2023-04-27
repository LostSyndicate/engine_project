#version 330 core

out vec4 frag;
in vec2 coord;
uniform sampler2D tex0;

void main()
{
	frag = vec4(texture(tex0, coord).rrr, 1.0);
}
