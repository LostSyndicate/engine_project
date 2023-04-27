#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_norm;
layout(location = 2) in vec2 a_coord;

out vec3 normal;
out vec3 fragPos;
out vec2 coord;
out vec4 fragLightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

void main()
{
	fragPos = vec3(model * vec4(a_pos, 1.0));
	normal = transpose(inverse(mat3(model))) *a_norm;  //mat3(transpose(inverse(model))) * a_norm;
	coord = a_coord;
	fragLightPos = lightSpace * vec4(fragPos, 1.0);
	gl_Position = projection * view * vec4(fragPos, 1.0);
}