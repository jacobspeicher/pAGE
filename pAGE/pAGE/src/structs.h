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