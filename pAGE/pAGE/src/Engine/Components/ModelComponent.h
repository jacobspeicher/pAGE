#pragma once

#include "../../structs.h"
#include "../../Utils/shader.h"

struct ModelComponent : IComponent {
public:
	unsigned int vao;
	Shader* shader;

	ModelComponent(const unsigned int& vao, Shader& shader) {
		this->vao = vao;
		this->shader = &shader;
		this->name = "Model Component";
	}
};