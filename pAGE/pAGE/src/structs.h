#pragma once

#include <glm/glm.hpp>

#include <string>

struct Project {
	std::string name;
	std::string path;

	Project() = default;
	Project(std::string name, std::string path) : name(name), path(path) {}
	Project(Project& other) : name(other.name), path(other.path) {}
};

struct IComponent {
	std::string name;
};

struct IOpenGLObject {
public:
	unsigned int vao;
	unsigned int vbo;
	std::vector<std::vector<glm::vec3>> triangles;

	virtual std::vector<std::vector<glm::vec3>> GetTriangles() = 0;

private:
	virtual void LoadTriangles() = 0;
};