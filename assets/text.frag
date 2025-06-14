#version 430

// Samplers
layout(binding = 0) uniform sampler2D uTexture;

// Inputs from vertex shader
in vec2 outTexCoords;
in vec4 outColor;

out vec4 oColor;

void main()
{
	// Calculate alpha based on the glyph texture so we dont get
	// black squares behind each letter of text
	float alpha = texture2D(uTexture, outTexCoords).a;
	vec4 textColor = clamp(outColor, 0.0, 1.0);
	oColor = vec4(textColor.rgb * textColor.a, textColor.a) * alpha;
}