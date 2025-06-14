#include "shapes.hpp"

#include "../vmlib/mat33.hpp"

// Method to create a cylinder
TexturelessSimpleMeshData makeCylinder(std::size_t subDivs, Vec3f color, Mat44f preTransform, bool isCapped)
{
	Mat33f const N = mat44_to_mat33(transpose(invert(preTransform)));

	std::vector<Vec3f> pos;
	std::vector<Vec3f> normals;

	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);

	for (std::size_t i = 0; i < subDivs; i++)
	{
		float const angle = (i + 1) / float(subDivs) * 2.f * 3.1415926f;
		float y = std::cos(angle);
		float z = std::sin(angle);

		// Caps
		if (isCapped)
		{
			pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
			pos.emplace_back(Vec3f{ 0.f, y, z });
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			normals.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			normals.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			normals.emplace_back(Vec3f{ 1.f, 0.f, 0.f });


			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ 1.f, y, z });
			pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			normals.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
			normals.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
			normals.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
		}

		// Length
		pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
		pos.emplace_back(Vec3f{ 0.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		normals.emplace_back(N * normalize(Vec3f{ 0.f, prevY, prevZ }));
		normals.emplace_back(N * normalize(Vec3f{ 0.f, y, z }));
		normals.emplace_back(N * normalize(Vec3f{ 0.f, prevY, prevZ }));

		pos.emplace_back(Vec3f{ 0.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		normals.emplace_back(N * normalize(Vec3f{ 0.f, y, z }));
		normals.emplace_back(N * normalize(Vec3f{ 0.f, y, z }));
		normals.emplace_back(N * normalize(Vec3f{ 0.f, prevY, prevZ }));

		prevY = y;
		prevZ = z;
	}

	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = preTransform * p4;
		t /= t.w;

		p = Vec3f{ t.x, t.y, t.z };
	}

	std::vector col(pos.size(), color);

	return TexturelessSimpleMeshData{ std::move(pos), std::move(col), std::move(normals) };
}

// Method to create a cone
TexturelessSimpleMeshData makeCone(std::size_t subDivs, Vec3f color, Mat44f preTransform, bool isCapped)
{
	Mat33f const N = mat44_to_mat33(transpose(invert(preTransform)));

	std::vector<Vec3f> pos;
	std::vector<Vec3f> normals;

	float prevY = std::cos(0.0f);
	float prevZ = std::sin(0.0f);

	for (std::size_t i = 0; i < subDivs; i++)
	{
		float const angle = (i + 1) / float(subDivs) * 2.f * 3.1415926f;
		float y = std::cos(angle);
		float z = std::sin(angle);

		// Caps
		if (isCapped)
		{
			pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
			pos.emplace_back(Vec3f{ 0.f, y, z });
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			normals.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			normals.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			normals.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
		}

		// Body
		pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
		pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
		pos.emplace_back(Vec3f{ 0.f, y, z });
		normals.emplace_back(N * normalize(Vec3f{ -1.f, 0.f, 0.f }));
		normals.emplace_back(N * normalize(Vec3f{ 0.f, prevY, prevZ }));
		normals.emplace_back(N * normalize(Vec3f{ 0.f, y, z }));

		prevY = y;
		prevZ = z;
	}

	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = preTransform * p4;
		t /= t.w;

		p = Vec3f{ t.x, t.y, t.z };
	}

	std::vector<Vec3f> col(pos.size(), color);

	return TexturelessSimpleMeshData{ std::move(pos), std::move(col), std::move(normals) };
}

// Method to create a pyramid
TexturelessSimpleMeshData makePyramid(Vec3f color, Mat44f preTransform)
{
	Mat33f const N = mat44_to_mat33(transpose(invert(preTransform)));

	std::vector<Vec3f> pos;
	std::vector<Vec3f> normals;

	// Front face
	Vec3f p1 = Vec3f{ 0.f, 1.f, 0.f };
	Vec3f p2 = Vec3f{ -1.f, 0.f, 1.f };
	Vec3f p3 = Vec3f{ 1.f, 0.f, 1.f };
	Vec3f normal = cross(p2 - p1, p3 - p1);
	pos.emplace_back(p1);
	pos.emplace_back(p2);
	pos.emplace_back(p3);
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });

	// Left face
	p1 = Vec3f{ 0.f, 1.f, 0.f };
	p2 = Vec3f{ -1.f, 0.f, -1.f };
	p3 = Vec3f{ -1.f, 0.f, 1.f };
	normal = cross(p2 - p1, p3 - p1);
	pos.emplace_back(p1);
	pos.emplace_back(p2);
	pos.emplace_back(p3);
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });

	// Back face
	p1 = Vec3f{ 0.f, 1.f, 0.f };
	p2 = Vec3f{ 1.f, 0.f, -1.f };
	p3 = Vec3f{ -1.f, 0.f, -1.f };
	normal = cross(p2 - p1, p3 - p1);
	pos.emplace_back(p1);
	pos.emplace_back(p2);
	pos.emplace_back(p3);
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });

	// Right face
	p1 = Vec3f{ 0.f, 1.f, 0.f };
	p2 = Vec3f{ 1.f, 0.f, 1.f };
	p3 = Vec3f{ 1.f, 0.f, -1.f };
	normal = cross(p2 - p1, p3 - p1);
	pos.emplace_back(p1);
	pos.emplace_back(p2);
	pos.emplace_back(p3);
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });

	// Bottom face (Needs 2 triangles since its a square)
	p1 = Vec3f{ 1.f, 0.f, 1.f };
	p2 = Vec3f{ -1.f, 0.f, 1.f };
	p3 = Vec3f{ -1.f, 0.f, -1.f };
	normal = cross(p2 - p1, p3 - p1);
	pos.emplace_back(p1);
	pos.emplace_back(p2);
	pos.emplace_back(p3);
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });

	p1 = Vec3f{ 1.f, 0.f, 1.f };
	p2 = Vec3f{ -1.f, 0.f, -1.f };
	p3 = Vec3f{ 1.f, 0.f, -1.f };
	normal = cross(p2 - p1, p3 - p1);
	pos.emplace_back(p1);
	pos.emplace_back(p2);
	pos.emplace_back(p3);
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });
	normals.emplace_back(Vec3f{ normal.x, normal.y, normal.z });

	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = preTransform * p4;
		t /= t.w;

		p = Vec3f{ t.x, t.y, t.z };
	}

	std::vector<Vec3f> col(pos.size(), color);

	return TexturelessSimpleMeshData{ std::move(pos), std::move(col), std::move(normals) };
}