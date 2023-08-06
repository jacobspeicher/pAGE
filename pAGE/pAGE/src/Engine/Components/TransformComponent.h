#pragma once

#include <glm/glm.hpp>

#include "../../structs.h"

struct TransformComponent : IComponent {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

	TransformComponent(
		glm::vec3 position = glm::vec3(0),
		glm::vec3 scale = glm::vec3(1),
		glm::vec3 rotation = glm::vec3(0)
	) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
		this->name = "Transform Component";
	}
};