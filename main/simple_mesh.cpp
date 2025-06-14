#include "simple_mesh.hpp"

// Concatenates two SimpleMeshData's together
SimpleMeshData concatenate(SimpleMeshData rMesh, SimpleMeshData const& lMesh)
{
	rMesh.positions.insert(rMesh.positions.end(), lMesh.positions.begin(), lMesh.positions.end());
	rMesh.colors.insert(rMesh.colors.end(), lMesh.colors.begin(), lMesh.colors.end());
	rMesh.normals.insert(rMesh.normals.end(), lMesh.normals.begin(), lMesh.normals.end());
	rMesh.texcoords.insert(rMesh.texcoords.end(), lMesh.texcoords.begin(), lMesh.texcoords.end());

	return rMesh;
}

// Creates a VAO for a given SimpleMeshData
GLuint createVAO(SimpleMeshData const& aMeshData)
{
	// Positions Vertex Buffer Object
	GLuint positionVBO = 0;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

	// Colors Vertex Buffer Object
	GLuint colorVBO = 0;
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

	// Normals Vertex Buffer Object
	GLuint normalVBO = 0;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);

	// Texture Coordinates Buffer Object
	GLuint textureVBO = 0;
	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f), aMeshData.texcoords.data(), GL_STATIC_DRAW);

	// VAO Initialisation
	GLuint meshVAO = 0;
	glGenVertexArrays(1, &meshVAO);
	glBindVertexArray(meshVAO);

	// Bind each VBO and its respective attributes to the mesh VAO
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	// Unbind and delete buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVBO);
	glDeleteBuffers(1, &colorVBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &textureVBO);

	return meshVAO;
}


// Textureless variants of the methods above
TexturelessSimpleMeshData concatenate(TexturelessSimpleMeshData rMesh, TexturelessSimpleMeshData const& lMesh)
{
	rMesh.positions.insert(rMesh.positions.end(), lMesh.positions.begin(), lMesh.positions.end());
	rMesh.colors.insert(rMesh.colors.end(), lMesh.colors.begin(), lMesh.colors.end());
	rMesh.normals.insert(rMesh.normals.end(), lMesh.normals.begin(), lMesh.normals.end());

	return rMesh;
}

GLuint createVAO(TexturelessSimpleMeshData const& aMeshData)
{
	// Positions Vertex Buffer Object
	GLuint positionVBO = 0;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

	// Colors Vertex Buffer Object
	GLuint colorVBO = 0;
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

	// Normals Vertex Buffer Object
	GLuint normalVBO = 0;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);

	// VAO Initialisation
	GLuint meshVAO = 0;
	glGenVertexArrays(1, &meshVAO);
	glBindVertexArray(meshVAO);

	// Bind each VBO and its respective attributes to the mesh VAO
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Unbind and delete buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVBO);
	glDeleteBuffers(1, &colorVBO);
	glDeleteBuffers(1, &normalVBO);

	return meshVAO;
}