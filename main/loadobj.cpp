#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

// Method for loading .obj files
SimpleMeshData loadWavefrontOBJ(char const* path)
{
	// Parse obj file
	auto result = rapidobj::ParseFile(path);
	if (result.error)
	{
		throw Error("Unable to load wavefront OBJ file '%s': '%s'", path, result.error.code.message().c_str());
	}

	// Triangulate vertices
	rapidobj::Triangulate(result);

	SimpleMeshData mesh;

	for (auto const& shape : result.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			// Put all positions from obj file into our mesh
			mesh.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
			});

			// Put colors from obj file into our mesh
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];

			mesh.colors.emplace_back(Vec3f{ mat.ambient[0], mat.ambient[1], mat.ambient[2] });

			// Put all normals from obj file into our mesh
			mesh.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
			});

			// Put all texture coordinates from obj file into mesh
			mesh.texcoords.emplace_back(Vec2f{
				result.attributes.texcoords[idx.texcoord_index * 2 + 0],
				result.attributes.texcoords[idx.texcoord_index * 2 + 1]
			});
		}
	}

	return mesh;
}
