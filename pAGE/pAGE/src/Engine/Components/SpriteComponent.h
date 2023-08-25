#pragma once

#include "structs.h"

struct SpriteComponent : ISpriteComponent {
public:
	SpriteComponent(std::shared_ptr<Shader> shader) {
		this->name = "Sprite Component";
		this->shader = shader;
	}
};