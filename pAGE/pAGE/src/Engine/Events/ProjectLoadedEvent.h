#pragma once

#include <string>

#include "../EventBus/Event.h"

#include "../../structs.h"

class ProjectLoadedEvent : public Event {
public:
	Project project;

	ProjectLoadedEvent(Project project) {
		this->project = project;
	}
};