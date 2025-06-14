#version 430

// Uniforms
layout(binding = 0) uniform sampler2D uTexture;

out vec4 oColor;

void main()
{
	// Color based on texture, just a white square for our particles.
	oColor = texture(uTexture, gl_PointCoord);
}