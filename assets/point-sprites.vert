#version 430

// VAO attributes
layout(location = 0) in vec3 iPosition;

// Uniforms
layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projectionMatrix;
layout(location = 3) uniform vec3 uCameraPos;

void main()
{
	// Change position based on the transformed model matrix from the application
	vec4 newPos = modelMatrix * vec4(iPosition, 1.0);

	// Calculate distance to particles to scale them correctly based on distance to camera
	float distToParticle = distance(uCameraPos, newPos.xyz);
	float scale = 1.0 - (distToParticle / 100.0);

	// Change the size of the point sprites based on distance
	gl_PointSize = 5.0 * scale;

	// Do usual vertex position transformation
	mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	gl_Position = mvpMatrix * vec4(iPosition, 1.0);
}