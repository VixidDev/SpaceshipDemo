#version 430

// Uniforms
layout(location = 2) uniform vec3 uLightPositions[3];
layout(location = 5) uniform vec3 uLightColors[3];
layout(location = 8) uniform vec3 uViewPos;

// Inputs from vertex shader
in vec3 outColor;
in vec3 outPos;
in vec3 v3fNormal;

// Output color
out vec4 oColor;

vec3 blinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
	// diffuse
	vec3 lightDirection = normalize(lightPos - fragPos);
	float diff = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDirection = normalize(uViewPos - fragPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	vec3 halfwayNormal = normalize(lightDirection + viewDirection);
	float spec = pow(max(dot(normal, halfwayNormal), 0.0), 32.0);
	vec3 specular = spec * lightColor;
	
	// Calculate distance attenuation
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0 / distance * distance;

	diffuse *= attenuation;
	specular *= attenuation;

	return diffuse + specular;
}

void main()
{
	vec3 lighting = vec3(0.0);
	vec3 color = outColor;

	// Add up the lighting from each light point
	for (int i = 0; i < 3; i++)
	{
		lighting += blinnPhong(normalize(v3fNormal), outPos, uLightPositions[i], uLightColors[i]);
	}
	color *= lighting;
	
	oColor = vec4(color, 1.0);
}