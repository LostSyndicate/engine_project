#version 330 core

out vec4 frag;
in vec3 pos;

void main()
{
	frag = vec4(normalize(pos), 1.0);
}