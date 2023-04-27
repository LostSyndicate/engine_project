#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 2) in vec2 a_coord;

out vec2 coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
	coord = a_coord;
}