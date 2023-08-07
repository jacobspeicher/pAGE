#pragma once

#include <memory>

#include "../Utils/shader.h"

#include "../../structs.h"

struct ModelComponent : IComponent {
public:
	unsigned int vao;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;

	ModelComponent(const unsigned int& vao, 
		std::shared_ptr<Shader> shader, 
		std::shared_ptr<Texture> texture) {
		this->vao = vao;
		this->shader = shader;
		this->texture = texture;
		this->name = "Model Component";
	}
};