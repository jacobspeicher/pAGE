#pragma once

#include <entt/entt.hpp>
#include <SDL.h>
#include <glew/glew.h>
#include <SDL_opengl.h>
#include <spdlog/spdlog.h>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imfilebrowser/imfilebrowser.h>

#include <memory>
#include <fstream>
#include <string>

#include "Objects/Object.h"
#include "EventBus/EventBus.h"

#include "Objects/Camera.h"
#include "../Utils/shader.h"
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
	/* SDL */
	SDL_Window* window;
	// OpenGL context for SDL window
	SDL_GLContext context;
	// stop render loop to close engine
	bool isRunning;
	// size of SDL window
	int windowWidth;
	int windowHeight;

	/* Engine */
	// define title of project and default path
	Project project;
	// event bus to handle emit/subscribe for events
	std::shared_ptr<EventBus> eventBus;
	// imgui filebrowser to create/open files
	std::unique_ptr<ImGui::FileBrowser> fileDialog;
	// entt registry to keep track of entities (which will also be `Object`s)
	entt::registry registry;
	// main camera for Scene
	Camera camera;

	/* UI */
	ImGuiIO* io;
	// list of objects in scene, used to populate Scene Hierarchy
	std::vector<Object> objects;
	// keep track of which object is selected in Scene Hierarchy
	int selected;
	// framebuffer and attachements, renders OpenGL to a texture to display in Scene window
	unsigned int fbo;
	unsigned int rbo;
	unsigned int texColorBuffer;
};

