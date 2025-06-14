#pragma once

#include "shapes.hpp"
#include "loadobj.hpp"

// VAOs and their respective vertex counts
GLuint parlahtiVAO, launchpadVAO, spaceshipVAO;
std::size_t parlahtiVertexCount, launchpadVertexCount, spaceshipVertexCount;

// Create the VAOs for various meshes
void makeVAOs()
{
	// PI constant
	constexpr float PI = 3.1415926f;

	// Creating VBO's and VAO's

	// Terrain VAO
	SimpleMeshData parlahtiMesh = loadWavefrontOBJ("assets/parlahti.obj");
	parlahtiVAO = createVAO(parlahtiMesh);
	parlahtiVertexCount = parlahtiMesh.positions.size();

	// Launchpad VAO
	SimpleMeshData launchpadMesh = loadWavefrontOBJ("assets/landingpad.obj");
	launchpadVAO = createVAO(launchpadMesh);
	launchpadVertexCount = launchpadMesh.positions.size();

	// Spaceship VAO
	// (Based on NASA's SLS Block 2 Cargo spaceship)

	// Advanced boosters (x2)
	// Made up of 2 cones and 1 cylinder
	TexturelessSimpleMeshData boosterConeOne = makeCone(128, Vec3f{ 1.f, 1.f, 1.f },
		make_translation(Vec3f{ -0.5f, -5.1f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(0.1f, 0.1f, 0.1f)
	);
	TexturelessSimpleMeshData mainCylinderOne = makeCylinder(32, Vec3f{ 1.f, 1.f, 1.f },
		make_translation(Vec3f{ -0.5f, -5.f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(4.f, 0.1f, 0.1f) // the x-axis is now the axis pointing up due to the rotation
	);
	TexturelessSimpleMeshData topConeOne = makeCone(32, Vec3f{ 1.f, 1.f, 1.f },
		make_translation(Vec3f{ -0.5f, -1.f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(0.2f, 0.1f, 0.1f)
	);

	TexturelessSimpleMeshData boosterOne = concatenate(std::move(concatenate(std::move(boosterConeOne), mainCylinderOne)), topConeOne);

	TexturelessSimpleMeshData boosterConeTwo = makeCone(32, Vec3f{ .8f, .8f, .8f },
		make_translation(Vec3f{ 0.5f, -5.1f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(0.1f, 0.1f, 0.1f)
	);
	TexturelessSimpleMeshData mainCylinderTwo = makeCylinder(32, Vec3f{ 1.f, 1.f, 1.f },
		make_translation(Vec3f{ 0.5f, -5.f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(4.f, 0.1f, 0.1f) // the x-axis is now the axis pointing up due to the rotation
	);
	TexturelessSimpleMeshData topConeTwo = makeCone(32, Vec3f{ 1.f, 1.f, 1.f },
		make_translation(Vec3f{ 0.5f, -1.f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(0.2f, 0.1f, 0.1f)
	);

	TexturelessSimpleMeshData boosterTwo = concatenate(std::move(concatenate(std::move(boosterConeTwo), mainCylinderTwo)), topConeTwo);

	TexturelessSimpleMeshData boosters = concatenate(std::move(boosterOne), boosterTwo);

	// Core Stage and Interstage
	// Made up of 1 cylinder and 1 pyramid (for the engine, yes its a cone in real life but need
	// for 3 different shape requirement)
	TexturelessSimpleMeshData engineCone = makePyramid(Vec3f{ .8f, .8f, .8f },
		make_translation(Vec3f{ 0.f, -5.1f, 0.f }) *
		make_scaling(0.3f, 0.3f, 0.3f)
	);
	TexturelessSimpleMeshData coreCylinder = makeCylinder(32, Vec3f{ .7f, .4f, 0.f },
		make_translation(Vec3f{ 0.f, -5.f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(5.f, 0.4f, 0.4f)
	);

	TexturelessSimpleMeshData coreStage = concatenate(std::move(engineCone), coreCylinder);

	// Cargo fairing
	// Made up of 1 cone, 1 upside down cone and 1 cylinder
	TexturelessSimpleMeshData upsideCone = makeCone(32, Vec3f{ 1.f, 1.f, 1.f },
		make_rotation_z(270.f * (PI / 180.f)) *
		make_translation(Vec3f{ 0.5f, 0.f, 0.f }) *
		make_scaling(0.5f, 0.5f, 0.5f)
	);
	TexturelessSimpleMeshData fairingCylinder = makeCylinder(32, Vec3f{ 1.f, 1.f, 1.f },
		make_translation(Vec3f{ 0.f, -0.5f, 0.f }) *
		make_rotation_z(90.f * (PI / 180.f)) *
		make_scaling(1.f, 0.5f, 0.5f)
	);
	TexturelessSimpleMeshData tipCone = makeCone(32, Vec3f{ 1.f, 1.f, 1.f },
		make_rotation_z(90.f * (PI / 180.f)) *
		make_translation(Vec3f{ 0.5f, 0.f, 0.f }) *
		make_scaling(0.5f, 0.5f, 0.5f)
	);

	TexturelessSimpleMeshData coneFairing = concatenate(std::move(concatenate(std::move(upsideCone), fairingCylinder)), tipCone);
	TexturelessSimpleMeshData boosterAndEngine = concatenate(std::move(boosters), coreStage);

	TexturelessSimpleMeshData entireShip = concatenate(std::move(coneFairing), boosterAndEngine);

	spaceshipVAO = createVAO(entireShip);
	spaceshipVertexCount = entireShip.positions.size();
}

// Arbitrary vertex data for a rectangle
static const float rectangleVertexData[] =
{
	-1.f, -1.f,
	 1.f, -1.f,
	 1.f,  1.f,

	-1.f, -1.f,
	 1.f,  1.f,
	-1.f,  1.f
};

// Makes the VAO for a rectangle
GLuint makeRectangle()
{
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertexData), rectangleVertexData, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);

	return vao;
}

// Crude way of making a rectangle outline
static const float rectangleLinesVertexData[] =
{
	-1.02f,  1.05f,
	 1.02f,  1.05f,

	 1.02f,  1.05f,
	 1.02f, -1.07f,

	 1.02f, -1.07f,
	-1.02f, -1.07f,

	-1.02f, -1.07f,
	-1.02f,  1.05f
};

// VAO for rectangle outline lines
GLuint makeLines()
{
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleLinesVertexData), rectangleLinesVertexData, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);

	return vao;
}