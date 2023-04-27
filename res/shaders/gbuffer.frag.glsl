#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec2 texCoords;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D tDiffuse;

void main()
{
    gPosition = fragPos;
    gNormal = normalize(normal);
    gAlbedoSpec.rgb = texture(tDiffuse, texCoords).rgb;
    gAlbedoSpec.a = 1.0;
}