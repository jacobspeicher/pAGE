#pragma once

#include "structs.h"

struct SpriteComponent : ISpriteComponent {
public:
	SpriteComponent(std::shared_ptr<Sprite> sprite, std::shared_ptr<Texture> texture, std::shared_ptr<Shader> shader) {
		this->name = "Sprite Component";
		this->sprite = sprite;
		this->texture = texture;
		this->shader = shader;
	}
};