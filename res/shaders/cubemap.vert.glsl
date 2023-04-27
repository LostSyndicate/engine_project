#version 330 core
layout (location = 0) in vec3 a_pos;

uniform mat4 projection;
uniform mat4 view;

out vec3 coords;

void main()
{
	coords = a_pos;
	vec4 pos = projection * view * vec4(a_pos, 1.0);
	gl_Position = pos.xyww;
}