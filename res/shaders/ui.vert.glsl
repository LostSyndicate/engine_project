#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aCoord;
layout(location = 2) in vec4 aColor;

uniform mat4 projection;

out vec2 oCoord;
out vec4 oColor;

void main()
{
	gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);
	oCoord = aCoord;
	oColor = aColor;
}