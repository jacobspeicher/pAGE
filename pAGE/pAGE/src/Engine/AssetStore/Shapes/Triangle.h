#pragma once

#include <glew/glew.h>

struct Triangle {
public:
	unsigned int vao;
	unsigned int vbo;

	Triangle() {
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

private:
	float vertices[18] = {
		// pos					color
		-0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	0.0f, // left
		0.5f,	-0.5f,	0.0f,	0.0f,	1.0f,	0.0f, // top
		0.0f,	0.5f,	0.0f,	0.0f,	0.0f,	1.0f, // right
	};
};