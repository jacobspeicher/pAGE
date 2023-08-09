#pragma once

#include <memory>

#include "../Utils/shader.h"

#include "structs.h"

struct ShapeComponent : IOpenGLComponent {
public:
	ShapeComponent(const unsigned int& vao, std::shared_ptr<Shader> shader) {
		this->vao = vao;
		this->shader = shader;
		this->name = "Shape Component";
	}
};