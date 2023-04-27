#version 330 core
layout(location = 0) in vec3 a_pos;

uniform mat4 cam_matrix;
uniform mat4 model;

void main()
{
	gl_Position = cam_matrix * model * vec4(a_pos, 1.0);
}