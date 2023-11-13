#pragma once

#include <assimp/scene.h>

#include <glew/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "../Utils/Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct AssimpTexture {
	unsigned int id;
	std::string type;
	std::string path;
};

struct AABB {
	glm::vec3 min;
	glm::vec3 max;
};

class Mesh {
public:
	// mesh data
	unsigned int vao;
	unsigned int boxVao;
	AABB box;

	std::vector<Vertex> vertices;
	std::vector<glm::vec3> boundingBoxVertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> boundingBoxIndices;
	std::vector<std::vector<glm::vec3>> triangles;
	std::vector<AssimpTexture> textures;

	Mesh(std::vector<Vertex> vertices, 
		std::vector<unsigned int> indices, 
		std::vector<std::vector<glm::vec3>> triangles, 
		std::vector<AssimpTexture> textures) {
		this->vertices = vertices;
		this->indices = indices;
		this->triangles = triangles;
		this->textures = textures;

		SetupMesh();
	}

	void GenerateAABB() {
		box.min = glm::vec3(1000.0f);
		box.max = glm::vec3(-1000.0f);

		for (unsigned int i = 0; i < vertices.size(); ++i) {
			glm::vec3 position = vertices[i].Position;

			if (position.x < box.min.x) {
				box.min.x = position.x;
			}
			if (position.x > box.max.x) {
				box.max.x = position.x;
			}

			if (position.y < box.min.y) {
				box.min.y = position.y;
			}
			if (position.y > box.max.y) {
				box.max.y = position.y;
			}

			if (position.z < box.min.z) {
				box.min.z = position.z;
			}
			if (position.z > box.max.z) {
				box.max.z = position.z;
			}
		}

		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.min.y, box.min.z));
		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.max.y, box.min.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.max.y, box.min.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.min.y, box.min.z));

		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.min.y, box.max.z));
		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.max.y, box.max.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.max.y, box.max.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.min.y, box.max.z));

		boundingBoxIndices = {
			0, 1, 3, // back face
			1, 2, 3,

			5, 1, 6, // top face
			1, 2, 6,

			4, 5, 7, // front face
			5, 6, 7,

			4, 0, 7, // bottom face
			0, 3, 7,

			0, 1, 4, // left face
			1, 5, 4,

			7, 6, 3, // right face
			6, 2, 3,
		};

		SetupBoundingBox();
	}

private:
	unsigned int vbo, ebo;
	unsigned int boxVbo, boxEbo;

	void SetupMesh() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

	void SetupBoundingBox() {
		glGenVertexArrays(1, &boxVao);
		glGenBuffers(1, &boxVbo);
		glGenBuffers(1, &boxEbo);

		glBindVertexArray(boxVao);

		glBindBuffer(GL_ARRAY_BUFFER, boxVbo);
		glBufferData(GL_ARRAY_BUFFER, boundingBoxVertices.size() * sizeof(glm::vec3), &boundingBoxVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, boundingBoxIndices.size() * sizeof(unsigned int), &boundingBoxIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(0);
	}
};