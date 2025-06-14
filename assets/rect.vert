#version 430

// VAO attributes
layout(location = 0) in vec2 iPosition;

// Uniforms
layout(location = 0) uniform mat4 modelMatrix;

void main()
{
	// Only need to move rectangle vertex position based on model matrix
	// since we are not doing anything with 3D projection or otherwise.
	gl_Position = modelMatrix * vec4(iPosition, 0.0, 1.0);
}