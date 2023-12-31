#include "Engine.h"

#include "Utils/Globals/Globals.h"

#include "Objects/Sprite.h"

#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Utils/ComponentUIHelper.h"

#include "Systems/RenderSystem.h"
#include "Systems/RenderDebugBoxSystem.h"
#include "Systems/RenderUISystem.h"
#include "Systems/SelectObjectSystem.h"

Engine::Engine() {
	/* SDL */
	isRunning = false;
	windowWidth = 1920;
	windowHeight = 1080;

	/* Engine */
	deltaTime = 0.0f;
	lastFrame = 0.0f;

	/* UI */
	Globals::selected = -1;
	Globals::mouseIsCaptured = false;

	/* Debug */
	Globals::debug = false;

	spdlog::info("{0} Created", outputPrefix);
}

Engine::~Engine() {
	spdlog::info("{0} Destroyed", outputPrefix);
}

void Engine::Initialize(std::shared_ptr<EventBus>& eventBus, Project project) {
	// define GL (3.3) and GLSL (330) versions
	const char* glsl_version = "#version 330";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		spdlog::error("{0} SDL could not be initialized", outputPrefix);
		return;
	}

	this->project = project;
	std::string windowTitle = "pAGE : " + project.name;

	// create SDL window
	window = SDL_CreateWindow(
		windowTitle.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL
	);
	if (!window) {
		spdlog::error("{0} SDL Window could not be created", outputPrefix);
		return;
	}

	// create opengl context
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		spdlog::error("{0} OpenGL context could not be created", outputPrefix);
		return;
	}
	SDL_GL_MakeCurrent(window, context);

	// initialize glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		spdlog::error("{0} GLEW could not be initialized", outputPrefix);
		return;
	}

	// initialize imgui
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGuiIO* io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGuiFileBrowserFlags filebrowser_flags = ImGuiFileBrowserFlags_CreateNewDir |
		ImGuiFileBrowserFlags_EnterNewFilename;
	fileDialog = std::make_unique<ImGui::FileBrowser>(filebrowser_flags);
	this->eventBus = eventBus;

	isRunning = true;

	camera.Position = glm::vec3(0.0f, 10.0f, 10.0f);
}

void Engine::Destroy() {
	glDeleteFramebuffers(1, &fbo);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	spdlog::info("{0} Cleaned up", outputPrefix);
}

void Engine::Setup() {
	ModelLoader::LoadModels(assetStore);
	ShaderLoader::LoadShaders(assetStore);
	TextureLoader::LoadTextures(assetStore);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	assetStore.AddOpenGLBinding("sceneFramebuffer", std::make_shared<unsigned int>(fbo));

	// generate texture to use as color attachment 0 for framebuffer
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	assetStore.AddOpenGLBinding("texColorBuffer", std::make_shared<unsigned int>(texColorBuffer));

	// attach texture to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// generate texture to use as color attachment 1 for framebuffer
	glGenTextures(1, &selColorBuffer);
	glBindTexture(GL_TEXTURE_2D, selColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	assetStore.AddOpenGLBinding("selColorBuffer", std::make_shared<unsigned int>(selColorBuffer));

	// attach selection color texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, selColorBuffer, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// create depth and stencil attachments to renderbuffer objects (don't need to sample data from these buffers)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 720);

	// attach renderbuffer object to depth and stencil attachment of framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::error("{0} Framebuffer is not complete\n{1}", outputPrefix, glCheckFramebufferStatus(GL_FRAMEBUFFER));
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
}

void Engine::Run() {
	while (isRunning) {
		ProcessInput();
		Render();

		float currentFrame = (float)SDL_GetTicks64() * 0.001f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
}

void Engine::ProcessInput() {
	SDL_Event sdlEvent;
	// process every SDL_Event that SDL_PollEvent captures
	while (SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		switch (sdlEvent.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				isRunning = false;
			break;
		case SDL_KEYUP:
			if (sdlEvent.key.keysym.sym == SDLK_BACKQUOTE)
				Globals::debug = !Globals::debug;
			break;
		case SDL_MOUSEBUTTONUP:
			if (Globals::mouseIsCaptured && sdlEvent.button.button == 3) {
				Globals::mouseIsCaptured = false;
				SDL_SetRelativeMouseMode((SDL_bool)false);
				SDL_WarpMouseInWindow(window, windowWidth / 2, windowHeight / 2);
			}
			break;
		case SDL_MOUSEMOTION:
			if (Globals::mouseIsCaptured)
				CameraProcessMouse(sdlEvent.motion.xrel, sdlEvent.motion.yrel);
		}
	}
	if (Globals::mouseIsCaptured)
		CameraProcessKeyboard();
}

void Engine::CameraProcessMouse(Sint32 xposIn, Sint32 yposIn) {
	float xPos = static_cast<float>(xposIn);
	float yPos = static_cast<float>(yposIn);

	camera.ProcessMouseMovement(xPos, -yPos);
}

void Engine::CameraProcessKeyboard() {
	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_W] == 1)
		camera.ProcessKeyboard(CAMERA_MOVEMENT::FORWARD, deltaTime);
	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_S] == 1)
		camera.ProcessKeyboard(CAMERA_MOVEMENT::BACKWARD, deltaTime);
	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_A] == 1)
		camera.ProcessKeyboard(CAMERA_MOVEMENT::LEFT, deltaTime);
	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_D] == 1)
		camera.ProcessKeyboard(CAMERA_MOVEMENT::RIGHT, deltaTime);
}

void Engine::Render() {
	glViewport(0, 0, 1200, 720);
	// render to framebuffer to create texture for scene viewport
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	static const float editorClearColor[] = { .2f, .2f, .2f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, editorClearColor);
	static const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 1, white);
	static const float depth = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &depth);

	RenderSystem(registry, camera);
	if (Globals::debug)
		RenderDebugBoxSystem(registry, camera, assetStore);

	glViewport(0, 0, windowWidth, windowHeight);
	// render main UI
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	RenderUISystem(registry, assetStore, camera);

	SDL_GL_SwapWindow(window);
}