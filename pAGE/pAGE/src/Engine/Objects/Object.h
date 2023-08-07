#pragma once

#include <entt/entity/entity.hpp>
#include <spdlog/spdlog.h>

#include <string>
#include <vector>

#include "../../structs.h"

class Object {
public:
	std::string name;
	entt::entity entity;
	
	Object() = default;
	Object(entt::entity& entity) : entity(entity) {
		spdlog::info("Object {0} created", (long)entity);
	}
	~Object() {
		spdlog::info("Object destroyed");
	};
};