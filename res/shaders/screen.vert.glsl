#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aCoord;

out vec2 texCoords;

void main()
{
    texCoords = aCoord;
    gl_Position = vec4(aPos, 1.0);
}