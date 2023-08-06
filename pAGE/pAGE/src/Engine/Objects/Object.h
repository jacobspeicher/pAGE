#pragma once

#include <entt/entity/entity.hpp>
#include <vector>

#include "../../structs.h"

class Object {
public:
	entt::entity entity;
	std::vector<IComponent> components;
	
	Object() = default;
	Object(entt::entity& entity) : entity(entity) {}
	~Object() = default;
};