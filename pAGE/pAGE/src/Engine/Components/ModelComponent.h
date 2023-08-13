#pragma once

#include "structs.h"

struct ModelComponent : IModelComponent {
public:
	/*
	ModelComponent(const unsigned int& vao,
		std::vector<std::vector<glm::vec3>> triangles,
		std::shared_ptr<Shader> shader, 
		std::shared_ptr<Texture> texture) {
		this->vao = vao;
		this->triangles = triangles;
		this->shader = shader;
		this->texture = texture;
		this->name = "Model Component";
	}*/
	ModelComponent(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader) {
		this->model = model;
		this->shader = shader;
		this->name = "ModelComponent";
	}
};