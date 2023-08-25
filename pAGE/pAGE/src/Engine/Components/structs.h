#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

#include "../Objects/Model.h"
#include "../Utils/shader.h"
#include "../Utils/texture.h"

struct IComponent {
	std::string name;
};

struct IModelComponent : IComponent {
	std::shared_ptr<Model> model;
	std::shared_ptr<Shader> shader;
};

struct ISpriteComponent : IComponent {
	std::shared_ptr<Shader> shader;
};

struct IOpenGLComponent : IComponent {
	unsigned int vao;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	std::vector<std::vector<glm::vec3>> triangles;
};