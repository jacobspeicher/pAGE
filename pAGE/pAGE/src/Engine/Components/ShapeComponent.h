#pragma once

#include <memory>

#include "../Utils/shader.h"

#include "../../structs.h"

struct ShapeComponent : IComponent {
public:
	unsigned int vao;
	std::shared_ptr<Shader> shader;

	ShapeComponent(const unsigned int& vao, std::shared_ptr<Shader> shader) {
		this->vao = vao;
		this->shader = shader;
		this->name = "Shape Component";
	}
};