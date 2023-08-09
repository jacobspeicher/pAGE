#pragma once

#include <glew/glew.h>

#include "../../../structs.h"

struct Cube : IOpenGLObject {
public:
	Cube() {
		LoadTriangles();
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vao);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// interpret the vertex data for positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// interpret the vertex data for normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// interpret the vertex data for texture coords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	virtual std::vector<std::vector<glm::vec3>> GetTriangles() override {
		return triangles;
	}

private:
	float vertices[288] =
	{
		// pos					// cube face normals	 // texture coords
		-0.5f,	-0.5f,	-0.5f,   0.0f,   0.0f,  -1.0f,	 0.0f,	 0.0f,	// bottom left
		 0.5f,	-0.5f,	-0.5f,   0.0f,   0.0f,  -1.0f,	 1.0f,	 0.0f,	// bottom right
		 0.5f,	 0.5f,	-0.5f,   0.0f,   0.0f,  -1.0f,	 1.0f,	 1.0f,	// top right
		 0.5f,	 0.5f,	-0.5f,   0.0f,   0.0f,  -1.0f,	 1.0f,	 1.0f,	// top right
		-0.5f,	 0.5f,	-0.5f,   0.0f,   0.0f,  -1.0f,	 0.0f,	 1.0f,	// top left
		-0.5f,	-0.5f,	-0.5f,   0.0f,   0.0f,  -1.0f,	 0.0f,	 0.0f,	// bottom left

		-0.5f,	-0.5f,	 0.5f,   0.0f,   0.0f,   1.0f,	 0.0f,	 0.0f,	// bottom left
		 0.5f,	-0.5f,	 0.5f,   0.0f,   0.0f,   1.0f,	 1.0f,	 0.0f,	// bottom right
		 0.5f,	 0.5f,	 0.5f,   0.0f,   0.0f,   1.0f,	 1.0f,	 1.0f,	// top right
		 0.5f,	 0.5f,	 0.5f,   0.0f,   0.0f,   1.0f,	 1.0f,	 1.0f,	// top right
		-0.5f,	 0.5f,	 0.5f,   0.0f,   0.0f,   1.0f,	 0.0f,	 1.0f,	// top left
		-0.5f,	-0.5f,	 0.5f,   0.0f,   0.0f,   1.0f,	 0.0f,	 0.0f,	// bottom left

		-0.5f,	 0.5f,	 0.5f,  -1.0f,   0.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left
		-0.5f,	 0.5f,	-0.5f,  -1.0f,   0.0f,   0.0f,	 1.0f,	 0.0f,	// bottom right
		-0.5f,	-0.5f,	-0.5f,  -1.0f,   0.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		-0.5f,	-0.5f,	-0.5f,  -1.0f,   0.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		-0.5f,	-0.5f,	 0.5f,  -1.0f,   0.0f,   0.0f,	 0.0f,	 1.0f,	// top left
		-0.5f,	 0.5f,	 0.5f,  -1.0f,   0.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left

		 0.5f,	 0.5f,	 0.5f,   1.0f,   0.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left
		 0.5f,	 0.5f,	-0.5f,   1.0f,   0.0f,   0.0f,	 1.0f,	 0.0f,	// bottom right
		 0.5f,	-0.5f,	-0.5f,   1.0f,   0.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		 0.5f,	-0.5f,	-0.5f,   1.0f,   0.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		 0.5f,	-0.5f,	 0.5f,   1.0f,   0.0f,   0.0f,	 0.0f,	 1.0f,	// top left
		 0.5f,	 0.5f,	 0.5f,   1.0f,   0.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left

		-0.5f,	-0.5f,	-0.5f,   0.0f,   -1.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left
		 0.5f,	-0.5f,	-0.5f,   0.0f,   -1.0f,   0.0f,	 1.0f,	 0.0f,	// bottom right
		 0.5f,	-0.5f,	 0.5f,   0.0f,   -1.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		 0.5f,	-0.5f,	 0.5f,   0.0f,   -1.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		-0.5f,	-0.5f,	 0.5f,   0.0f,   -1.0f,   0.0f,	 0.0f,	 1.0f,	// top left
		-0.5f,	-0.5f,	-0.5f,   0.0f,   -1.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left

		-0.5f,	 0.5f,	-0.5f,   0.0f,    1.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left
		 0.5f,	 0.5f,	-0.5f,   0.0f,    1.0f,   0.0f,	 1.0f,	 0.0f,	// bottom right
		 0.5f,	 0.5f,	 0.5f,   0.0f,    1.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		 0.5f,	 0.5f,	 0.5f,   0.0f,    1.0f,   0.0f,	 1.0f,	 1.0f,	// top right
		-0.5f,	 0.5f,	 0.5f,   0.0f,    1.0f,   0.0f,	 0.0f,	 1.0f,	// top left
		-0.5f,	 0.5f,	-0.5f,   0.0f,    1.0f,   0.0f,	 0.0f,	 0.0f,	// bottom left
	};

	virtual void LoadTriangles() override {
		for (int i = 0; i < 36; i += 3) {
			int v1 = i * 8;
			int v2 = (i + 1) * 8;
			int v3 = (i + 2) * 8;

			glm::vec3 vert1(vertices[v1], vertices[v1 + 1], vertices[v1 + 2]);
			glm::vec3 vert2(vertices[v2], vertices[v2 + 1], vertices[v2 + 2]);
			glm::vec3 vert3(vertices[v3], vertices[v3 + 1], vertices[v3 + 2]);

			std::vector<glm::vec3> triangle = { vert1, vert2, vert3 };

			triangles.push_back(triangle);
		}
	}
};