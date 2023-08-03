#pragma once

#include <SDL.h>
#include <spdlog/spdlog.h>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#include <imfilebrowser/imfilebrowser.h>

#include<memory>

#include "../interfaces.h"

class Launcher : public IContainer {
public:
	Launcher();
	~Launcher();

	void Initialize() override;
	void Destroy() override;
	void Setup() override;
	void Run() override;
	void ProcessInput() override;
	void Render() override;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::unique_ptr<ImGui::FileBrowser> fileDialog;
	
	bool isRunning;

	int windowWidth;
	int windowHeight;
};