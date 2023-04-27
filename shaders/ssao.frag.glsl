#version 330 core

out float frag;

in vec3 fragPos;
in vec3 normal;
in mat4 proj;

uniform vec3 samples[64];
uniform sampler2D ssaoNoise;
const vec2 noiseScale = vec2(1280.0/4.0,720.0/4.0);

void main()
{
	vec3 norm = normalize(normal);
	vec3 randVec = texture(ssaoNoise, ).xyz;
}