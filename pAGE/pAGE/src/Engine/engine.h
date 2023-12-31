#pragma once

#include <entt/entt.hpp>
#include <glew/glew.h>
#include <SDL.h>
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

#include "AssetStore/AssetStore.h"
#include "EventBus/EventBus.h"
#include "Objects/Camera.h"
#include "Objects/Model.h"
#include "Utils/Shader.h"
#include "Utils/Loaders/ModelLoader/ModelLoader.h"
#include "Utils/Loaders/ShaderLoader/ShaderLoader.h"
#include "Utils/Loaders/TextureLoader/TextureLoader.h"

#include "../Structs.h"

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

	/* UI */
	void CameraProcessMouse(Sint32 xposIn, Sint32 yposIn);
	void CameraProcessKeyboard();
	void ShowInspector();
	void ShowSceneHierarchy();
	void ShowScene(ImTextureID texture);

private:
	/* debug message prefix */
	const std::string outputPrefix = "ENGINE :";

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
	// hold all default engine assets (models, textures, shaders, etc)
	AssetStore assetStore;
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
	// make sure rendering is done at consistent time steps
	float deltaTime;
	float lastFrame;
	
	// framebuffer and attachements, renders OpenGL to a texture to display in Scene window
	unsigned int fbo;
	unsigned int rbo;
	unsigned int texColorBuffer;
	// color attachment, renders object index to determine object selection
	unsigned int selColorBuffer;
};

