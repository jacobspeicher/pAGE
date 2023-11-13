#pragma once

#include <glew/glew.h>

#include "../../../Structs.h"

struct Triangle : IOpenGLObject {
public:
	Triangle() {
		LoadTriangles();
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vao);

		// copy vertex data into the Vertex Array Object
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// interpret the vertex data for positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// interpret the vertex data for colors and enable
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	virtual std::vector<std::vector<glm::vec3>> GetTriangles() override {
		return triangles;
	}

private:
	float vertices[18] = {
		// pos					color
		-0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	0.0f, // left
		0.5f,	-0.5f,	0.0f,	0.0f,	1.0f,	0.0f, // top
		0.0f,	0.5f,	0.0f,	0.0f,	0.0f,	1.0f, // right
	};

	virtual void LoadTriangles() override {
		glm::vec3 vert1(vertices[0], vertices[1], vertices[2]);
		glm::vec3 vert2(vertices[6], vertices[7], vertices[8]);
		glm::vec3 vert3(vertices[12], vertices[13], vertices[14]);
		std::vector<glm::vec3> triangle = { vert1, vert2, vert3 };

		triangles.push_back(triangle);
	}
};