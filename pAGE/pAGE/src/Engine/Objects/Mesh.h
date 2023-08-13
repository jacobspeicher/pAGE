#pragma once

#include <assimp/scene.h>

#include <glew/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "../Utils/shader.h"

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

class Mesh {
public:
	// mesh data
	unsigned int vao;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
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
	/*
	void Draw(Shader& shader) {
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;

		for (unsigned int i = 0; i < textures.size(); ++i) {
			// activate texture unit first
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			shader.SetFloat(("material." + name + number).c_str(), (float)i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glActiveTexture(GL_TEXTURE0);

		// draw mesh
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	*/

private:
	unsigned int vbo, ebo;

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
};