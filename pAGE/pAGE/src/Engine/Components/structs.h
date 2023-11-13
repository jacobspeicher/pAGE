#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

#include "../Objects/Model.h"
#include "../Objects/Sprite.h"

#include "../Utils/Shader.h"
#include "../Utils/Texture.h"

struct IComponent {
	std::string name;
};

struct IModelComponent : IComponent {
	std::shared_ptr<Model> model;
	std::shared_ptr<Shader> shader;
};

struct ISpriteComponent : IComponent {
	std::shared_ptr<Sprite> sprite;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Shader> shader;
};