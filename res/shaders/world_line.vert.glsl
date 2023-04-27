#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_norm;
layout(location = 2) in vec2 a_coord;

out vec3 pos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
	pos = a_pos;
}