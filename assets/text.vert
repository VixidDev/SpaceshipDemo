#version 430

// VAO attributes
layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec2 iTexCoord;
layout(location = 2) in vec4 iColor;

// Uniforms
layout(location = 0) uniform mat4 orthoMatrix;
layout(location = 1) uniform mat4 modelMatrix;

out vec2 outTexCoords;
out vec4 outColor;

void main()
{
	outTexCoords = iTexCoord;
	outColor = iColor;

	// Transform vertex position based on orthographic matrix and model matrix
	gl_Position = orthoMatrix * modelMatrix * vec4(iPosition, 0.0, 1.0);
}