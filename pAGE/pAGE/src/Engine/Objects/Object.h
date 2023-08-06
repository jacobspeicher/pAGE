#pragma once

#include <entt/entity/entity.hpp>

#include <string>
#include <vector>

#include "../../structs.h"

class Object {
public:
	std::string name;
	entt::entity entity;
	
	Object() = default;
	Object(entt::entity& entity) : entity(entity) {}
	~Object() = default;
};