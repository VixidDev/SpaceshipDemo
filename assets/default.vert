#version 430

// VAO attributes
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iColor;
layout(location = 2) in vec3 iNormal;
layout(location = 3) in vec2 iTexCoords;

// Uniforms
layout(location = 0) uniform mat4 uProjCameraWorld;

// Stuff to pass to fragment shader
out vec3 outColor;
out vec2 v2fTexCoord;

void main()
{
	// Setting fragment shader variables
	outColor = iColor;
	v2fTexCoord = iTexCoords;

	// Set vertex position
	gl_Position = uProjCameraWorld * vec4(iPosition, 1.0);
}
