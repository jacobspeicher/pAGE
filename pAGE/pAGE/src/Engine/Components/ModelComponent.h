#pragma once

#include <memory>

#include "../Utils/shader.h"

#include "../../structs.h"

struct ModelComponent : IComponent {
public:
	unsigned int vao;
	std::shared_ptr<Shader> shader;

	ModelComponent(const unsigned int& vao, std::shared_ptr<Shader> shader) {
		this->vao = vao;
		this->shader = shader;
		this->name = "Model Component";
	}
};