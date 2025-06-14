#pragma once

#include <vector>
#include <random>
#include <chrono>

#include "glad.h"
#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"
#include "../support/program.hpp"

// Struct to hold the position, velocities and lifetimes of all particles.
// A particle can be defined by the same index from all 3 attributes.
struct Particles
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> velocities;
	std::vector<float> lifeTimes;
};

// Class to deal with generating the particles and handling updating, creation and deletion.
class ParticleGenerator
{
public:
	ParticleGenerator(int maxParticles, Vec3f initialPosition);
	void initParticles();
	void resetParticles();
	void createVAO();
	void update(float deltaTime, Vec3f updatedShipPos);

	Particles particles;
	int maxParticles{};
	Vec3f conePosition;
	Vec3f coneDirection;
	Mat44f rotation;
	GLuint vao;

private:
	Vec3f getRandomVectorInCone(float angleDeviation);

	GLuint vbo;

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
};

