#pragma once

#include "structs.h"

struct ModelComponent : IModelComponent {
public:
	ModelComponent(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader) {
		this->model = model;
		this->shader = shader;
		this->name = "ModelComponent";
	}
};