#version 330 core

out vec4 frag;
in vec3 fragPos;
in vec3 normal;
in vec2 coord;
in vec4 fragLightPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float ambientStrength;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

vec2 poissonDisk[16] = vec2[](
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 ),
	vec2( -0.91588581, 0.45771432 ),
	vec2( -0.81544232, -0.87912464 ),
	vec2( -0.38277543, 0.27676845 ),
	vec2( 0.97484398, 0.75648379 ),
	vec2( 0.44323325, -0.97511554 ),
	vec2( 0.53742981, -0.47373420 ),
	vec2( -0.26496911, -0.41893023 ),
	vec2( 0.79197514, 0.19090188 ),
	vec2( -0.24188840, 0.99706507 ),
	vec2( -0.81409955, 0.91437590 ),
	vec2( 0.19984126, 0.78641367 ),
	vec2( 0.14383161, -0.14100790 )
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main()
{
	// 0.623, 0.776, 0.87
	vec3 lightColor = vec3(0.673, 0.806, 0.87);
	vec4 texColor = texture(diffuseTexture,  coord);
	if (texColor.a < 0.5)
		discard;
	vec3 objectColor = texColor.rgb;

	vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 projCoords = fragLightPos.xyz / fragLightPos.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)

		{
//			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			float pcfDepth = texture(shadowMap, projCoords.xy +
			poissonDisk[int(16.0 * random(gl_FragCoord.xyy, x + 1 + y + 1)) % 16] / 2700.0
									 + (vec2(x,y)*texelSize)
			).r;

			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	// ambient + (1.0 - shadow) * (diffuse + specular)
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;
	frag = vec4(lighting, 1.0);
}