// SimpleMeshData struct for meshes, same as that used in exercises for
// simplicity and familiarity
#pragma once

#include <glad.h>

#include <vector>

#include "../vmlib/vec2.hpp"
#include "../vmlib/vec3.hpp"

struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texcoords;
};

SimpleMeshData concatenate(SimpleMeshData, SimpleMeshData const&);

GLuint createVAO(SimpleMeshData const&);

// Use of a 'Textureless' simple mesh data to handle meshes that don't
// contain texture coordinates so we can correctly use them in shaders and
// with OpenGL functions
struct TexturelessSimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;
};

TexturelessSimpleMeshData concatenate(TexturelessSimpleMeshData, TexturelessSimpleMeshData const&);

GLuint createVAO(TexturelessSimpleMeshData const&);