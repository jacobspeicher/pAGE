#pragma once

#include <entt/entt.hpp>
#include <SDL.h>
#include <spdlog/spdlog.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#include <imfilebrowser/imfilebrowser.h>

#include <memory>
#include <fstream>
#include <string>

#include "EventBus/EventBus.h"

#include "../structs.h"

class Engine {
public:
	Engine();
	~Engine();

	void Initialize(std::shared_ptr<EventBus>& eventBus, Project project);
	void Destroy();
	void Setup();
	void Run();
	void ProcessInput();
	void Render();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::shared_ptr<EventBus> eventBus;
	std::unique_ptr<ImGui::FileBrowser> fileDialog;

	bool isRunning;

	int windowWidth;
	int windowHeight;

	Project project;
};

