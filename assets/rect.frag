#version 430

// Uniforms
layout(location = 1) uniform vec4 color;

out vec4 oColor;

void main()
{
	oColor = color;
}