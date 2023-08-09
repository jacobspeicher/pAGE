#pragma once

#include <memory>

#include "structs.h"

struct ModelComponent : IOpenGLComponent {
public:
	ModelComponent(const unsigned int& vao,
		std::vector<std::vector<glm::vec3>> triangles,
		std::shared_ptr<Shader> shader, 
		std::shared_ptr<Texture> texture) {
		this->vao = vao;
		this->triangles = triangles;
		this->shader = shader;
		this->texture = texture;
		this->name = "Model Component";
	}
};