#pragma once

#include <glew/glew.h>

class Sprite {
public:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	int numVertices;

	Sprite() {
		vao = 0;
		vbo = 0;
		ebo = 0;
		numVertices = 20;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vao);

		// copy vertex data into the Vertex Array Object
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// interpret the vertex data for positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	
private:
	float vertices[20] = {
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int indices[6] = {
		0, 1, 3,
		1, 2, 3,
	};
};