#pragma once

#include <SDL.h>
#include <spdlog/spdlog.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#include <imfilebrowser/imfilebrowser.h>

#include<memory>
#include <fstream>

#include "../Engine/EventBus/EventBus.h"
#include "../Engine/Events/ProjectLoadedEvent.h"

#include "../Structs.h"

class Launcher {
public:
	Launcher();
	~Launcher();

	void Initialize(std::shared_ptr<EventBus>& eventBus);
	void Destroy();
	void Setup();
	void Run();
	void ProcessInput();
	void Render();

private:
	void ShowActionsWindow();
	void ShowProjectsWindow();

	SDL_Window* window;
	SDL_Renderer* renderer;

	std::shared_ptr<EventBus> eventBus;
	std::unique_ptr<ImGui::FileBrowser> fileDialog;
	
	bool isRunning;

	int windowWidth;
	int windowHeight;

	int selected;
	Project selectedProject;
};