#pragma once

#include <string>

#include "structs.h"

struct UIEntityNameComponent : IComponent {
	std::string entityName;

	UIEntityNameComponent(std::string name = "Entity") {
		this->entityName = name;
		this->name = "Entity Name";
	}
};