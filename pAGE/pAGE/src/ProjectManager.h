#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <string>

#include "Engine/Engine.h"
#include "Launcher/Launcher.h"

#include "Engine/EventBus/EventBus.h"
#include "Engine/Events/ProjectLoadedEvent.h"

#include "Structs.h"

class ProjectManager {
public:
	ProjectManager() {
		launcher = true;
		spdlog::info("{0} Created", outputPrefix);
	}
	~ProjectManager() {
		spdlog::info("{0} Destroyed", outputPrefix);
	}

	void Initialize() {
		eventBus = std::make_shared<EventBus>();
		eventBus->SubscribeToEvent<ProjectLoadedEvent>(this,  &ProjectManager::LoadProject);
	}

	void Run() {
		if (launcher) {
			spdlog::info("{0} Running launcher", outputPrefix);
			Launcher launcher;
			launcher.Initialize(eventBus);
			launcher.Run();
			launcher.Destroy();
		}
		if (!launcher) {
			spdlog::info("{0} Running engine", outputPrefix);
			Engine engine;
			engine.Initialize(eventBus, project);
			engine.Setup();
			engine.Run();
			engine.Destroy();
		}
	}

	void LoadProject(ProjectLoadedEvent& event) {
		project = event.project;
		launcher = false;
		spdlog::info("{0} {1} ({2})", outputPrefix, project.name, project.path);
	}

private:
	/* debug message prefix */
	const std::string outputPrefix = "PROJECT MANAGER :";

	std::shared_ptr<EventBus> eventBus;

	Project project;

	bool launcher;
};