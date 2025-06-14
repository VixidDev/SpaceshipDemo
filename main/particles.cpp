#include "particles.hpp"

// PI constant
constexpr float PI = 3.1415926f;

ParticleGenerator::ParticleGenerator(int maxParticles, Vec3f initialPosition) : maxParticles(maxParticles), conePosition(initialPosition), coneDirection(Vec3f{0.f, -1.f, 0.f})
{
	// Initialise random number generator using uniform real distribution
	unsigned seed = (unsigned int) std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen(seed);
	this->generator = gen;

	std::uniform_real_distribution<float> dist(0.f, 1.f);
	this->distribution = dist;

	this->initParticles();
}

void ParticleGenerator::initParticles()
{
	// Create maxParticles amount of particles by filling the 3 std::vectors in Particles struct
	for (int i = 0; i < this->maxParticles; i++)
	{
		this->particles.positions.emplace_back(conePosition);

		// Gets a random velocity for each particle base on the cone direction of the rocket
		Vec3f velocity;
		velocity = this->getRandomVectorInCone(0.8f);
		velocity = normalize(velocity);
		velocity = velocity * 5.f;
		this->particles.velocities.emplace_back(velocity);

		this->particles.lifeTimes.emplace_back(2.f * this->distribution(this->generator));
	}
}

void ParticleGenerator::resetParticles()
{
	// Reset particles when rocket animation is reset and recalculate particles
	this->particles.positions.clear();
	this->particles.velocities.clear();
	this->particles.lifeTimes.clear();

	this->initParticles();
}

// Calculates a random vector in a conical shape based on
// https://math.stackexchange.com/a/205589
Vec3f ParticleGenerator::getRandomVectorInCone(float deviation)
{
	float cosAngle = cos(deviation);
	float theta = (this->distribution(this->generator) * 2.f * PI);
	float z = cosAngle + (this->distribution(this->generator) * (1 - cosAngle));
	float root1MinusSq = sqrt(1 - z * z);
	
	float x = root1MinusSq * cos(theta);
	float y = root1MinusSq * sin(theta);

	Vec4f direction = Vec4f{ x, y, z, 1 };

	Vec3f rotateAxis = normalize(cross(this->coneDirection, Vec3f{ 0.f, 0.f, 1.f }));
	float rotateAngle = acos(dot(this->coneDirection, Vec3f{ 0.f, 0.f, 1.f }));
	Mat44f rotationMatrix = rotate(-rotateAngle, rotateAxis);
	direction = rotationMatrix * direction;

	Vec3f newDir = Vec3f{ direction.x, direction.y, direction.z };

	return newDir;
}

// Create VAO for the particle positions
void ParticleGenerator::createVAO()
{
 	this->vbo = 0;
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->particles.positions.size() * sizeof(Vec3f), this->particles.positions.data(), GL_STATIC_DRAW);

	this->vao = 0;
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// We don't delete the VBO here like we would usually,
	// since we need it to continue to exist later on
	// when we need to update the positions in the VBO
	// using glMapBuffer.
}

void ParticleGenerator::update(float deltaTime, Vec3f updatedShipPos)
{
	// Iterates over all particles and updates their position and life time
	for (int i = 0; i < this->maxParticles; i++)
	{
		// Check for 'dead' particles
		if (this->particles.lifeTimes[i] < 0.f)
		{
			this->particles.positions[i] = updatedShipPos;

			Vec3f velocity;
			velocity = this->getRandomVectorInCone(0.8f);
			velocity = normalize(velocity);
			velocity = velocity * 5.f;
			this->particles.velocities[i] = velocity;

			this->particles.lifeTimes[i] = 2.f * this->distribution(this->generator);
		}
		// Otherwise just update as normal
		else
		{
			Vec3f change = this->particles.velocities[i] * deltaTime;
			this->particles.positions[i] += change;
			this->particles.lifeTimes[i] -= deltaTime;
		}
	}

	// Update data in VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr)
	{
		std::memcpy(ptr, this->particles.positions.data(), this->particles.positions.size() * sizeof(Vec3f));
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}