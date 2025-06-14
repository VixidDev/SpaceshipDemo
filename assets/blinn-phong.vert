#version 430

// VAO attributes
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iColor;
layout(location = 2) in vec3 iNormal;

// Uniforms
layout(location = 0) uniform mat4 uProjCameraWorld;
layout(location = 1) uniform mat3 uNormalMatrix;

// Stuff to pass to fragment shader
out vec3 outColor;
out vec3 outPos;
out vec3 v3fNormal;

void main()
{
	// Setting fragment shader variables
	outColor = iColor;
	outPos = iPosition;
	v3fNormal = normalize(uNormalMatrix * iNormal);
	
	// Set vertex position
	gl_Position = uProjCameraWorld * vec4(iPosition, 1.0);
}