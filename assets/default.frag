#version 430

// Samplers
layout(binding = 0) uniform sampler2D uTexture;

// Inputs from vertex shader
in vec3 outColor;
in vec2 v2fTexCoord;

// Color output
out vec3 oColor;

void main()
{
	// Output color based on texture
	oColor = texture(uTexture, v2fTexCoord).rgb;
}

