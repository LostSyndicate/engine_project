#version 330 core

out vec4 frag;
in vec3 coords;

uniform samplerCube skybox;

void main()
{
	frag = texture(skybox, coords);
}