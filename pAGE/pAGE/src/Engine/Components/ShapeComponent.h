#pragma once

#include <memory>

#include "../Utils/Shader.h"

#include "Structs.h"

struct ShapeComponent : IOpenGLComponent {
public:
	ShapeComponent(const unsigned int& vao, std::shared_ptr<Shader> shader) {
		this->vao = vao;
		this->shader = shader;
		this->name = "Shape Component";
	}
};