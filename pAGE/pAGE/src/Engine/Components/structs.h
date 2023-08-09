#pragma once

#include <glm/glm.hpp>

#include <string>

#include "../Utils/shader.h"
#include "../Utils/texture.h"

struct IComponent {
	std::string name;
};

struct IOpenGLComponent : IComponent {
	unsigned int vao;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	std::vector<std::vector<glm::vec3>> triangles;
};