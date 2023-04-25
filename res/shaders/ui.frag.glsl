#version 330 core

out vec4 frag;

in vec2 oCoord;
in vec4 oColor;

uniform sampler2D uTexture;
uniform int uIsText;

void main()
{
	vec4 col = uIsText == 1 ? vec4(texture(uTexture, oCoord).r) : texture(uTexture, oCoord);
	if (col.a < 0.5)
		discard;
	frag = col * oColor;
}