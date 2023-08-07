#include "engine.h"

#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"
#include "Components/ShapeComponent.h"

#include "Systems/RenderSystem.h"

#include "Components/ComponentUI.h"

Engine::Engine() {
	/* SDL */
	isRunning = false;
	windowWidth = 1920;
	windowHeight = 1080;

	/* Engine */
	deltaTime = 0.0f;
	lastFrame = 0.0f;

	/* UI */
	selected = -1;
	mouseIsCaptured = false;
	lastX = (float)windowWidth / 2.0f;
	lastY = (float)windowHeight / 2.0f;
	firstMouse = true;

	spdlog::info("Engine created");
}

Engine::~Engine() {
	spdlog::info("Engine destroyed");
}

void Engine::Initialize(std::shared_ptr<EventBus>& eventBus, Project project) {
	// define GL (3.3) and GLSL (330) versions
	const char* glsl_version = "#version 330";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		spdlog::error("SDL could not be initialized");
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
		spdlog::error("SDL Window could not be created");
		return;
	}

	// create opengl context
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		spdlog::error("OpenGL context could not be created");
		return;
	}
	SDL_GL_MakeCurrent(window, context);

	// initialize glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		spdlog::error("GLEW could not be initialized");
		return;
	}

	// initialize imgui
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGuiFileBrowserFlags filebrowser_flags = ImGuiFileBrowserFlags_CreateNewDir |
		ImGuiFileBrowserFlags_EnterNewFilename;
	fileDialog = std::make_unique<ImGui::FileBrowser>(filebrowser_flags);
	this->eventBus = eventBus;

	isRunning = true;

	camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
}

void Engine::Destroy() {
	glDeleteFramebuffers(1, &fbo);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	spdlog::info("Engine cleaned up");
}

void Engine::Setup() {
	OpenGLObjectsLoader::LoadOpenGLObjects(assetStore);
	ShaderLoader::LoadShaders(assetStore);
	TextureLoader::LoadTextures(assetStore);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture to use as color attachment for framebuffer
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach texture to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// create depth and stencil attachments to renderbuffer objects (don't need to sample data from these buffers)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 720);

	// attach renderbuffer object to depth and stencil attachment of framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::error("Framebuffer is not complete\n{0}", glCheckFramebufferStatus(GL_FRAMEBUFFER));
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
			if (sdlEvent.key.keysym.sym == SDLK_F12) {
				if (sdlEvent.key.keysym.mod & KMOD_SHIFT) {
					mouseIsCaptured = !mouseIsCaptured;
					SDL_SetRelativeMouseMode((SDL_bool)mouseIsCaptured);
					if (!mouseIsCaptured) {
						firstMouse = true;
						SDL_WarpMouseInWindow(window, windowWidth / 2, windowHeight / 2);
					}
				}
			}
			break;
		case SDL_MOUSEMOTION:
			if (mouseIsCaptured)
				CameraProcessMouse(sdlEvent.motion.xrel, sdlEvent.motion.yrel);
		}
	}
	if (mouseIsCaptured)
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
	glClearColor(.2f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderSystem(registry, camera);

	glViewport(0, 0, windowWidth, windowHeight);
	// render main UI
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ShowSceneHierarchy();
	ShowInspector();
	ShowScene((ImTextureID)texColorBuffer);

	ImGui::Render(); 
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}

void Engine::ShowInspector() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowPos(ImVec2(1578, 10));
	ImGui::SetNextWindowSize(ImVec2(332, 755));

	ImGui::Begin("Inspector", NULL, window_flags);

	if (selected != -1) {
		Object& object = objects[selected];
		
		std::string title = "Object : " + object.name;
		ImGui::SeparatorText(title.c_str());

		ImGui::InputText("name", &object.name);
		ImGui::Spacing();

		if (registry.all_of<TransformComponent>(object.entity)) {
			ComponentUI::PopulateTransformComponent(registry.get<TransformComponent>(object.entity));
		}
		if (registry.all_of<ModelComponent>(object.entity)) {
			ComponentUI::PopulateModelComponent(registry.get<ModelComponent>(object.entity));
		}
		if (registry.all_of<ShapeComponent>(object.entity)) {
			ComponentUI::PopulateShapeComponent(registry.get<ShapeComponent>(object.entity));
		}
	}

	ImGui::End();
}

void Engine::ShowSceneHierarchy() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_MenuBar;
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(332, 755));

	ImGui::Begin("Scene Hierarchy", NULL, window_flags);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("2D")) {
			if (ImGui::MenuItem("Triangle")) {
				auto triangle = registry.create();
				Object object(triangle);
				object.name = "Triangle (" + std::to_string((long)triangle) + ")";
				registry.emplace<TransformComponent>(triangle, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f));
				registry.emplace<ShapeComponent>(triangle, assetStore.GetOpenGLObject("triangle")->vao, assetStore.GetShader("basic2D"));
				objects.push_back(object);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("3D")) {
			if (ImGui::MenuItem("Cube")) {
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");

				auto cube = registry.create();
				Object object(cube);
				object.name = "Cube (" + std::to_string((long)cube) + ")";
				registry.emplace<TransformComponent>(cube, glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(cube, 
					assetStore.GetOpenGLObject("cube")->vao, 
					assetStore.GetShader("basic3D"),
					assetStore.GetTexture("container"));
				objects.push_back(object);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	int i = 0;
	for (Object object : objects) {
		std::string title = std::to_string((long)object.entity) + " : " + object.name;
		if (ImGui::Selectable(title.c_str(), selected == i)) {
			selected = i;
		}
		++i;
	}
	
	ImGui::End();
}

void Engine::ShowScene(ImTextureID texture) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGui::SetNextWindowPos(ImVec2(352, 10));
	ImGui::SetNextWindowSize(ImVec2(1216, 755));

	ImGuiWindowFlags overlay_window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove;

	ImGui::Begin("Scene", NULL, window_flags);

	ImGui::Image(texture, ImVec2(1200, 720), ImVec2(0, 1), ImVec2(1, 0));

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 childWindowPos, childWindowPosPivot;
	childWindowPos.x = windowPos.x + windowSize.x - 10.0f;
	childWindowPos.y = windowPos.y + 30.0f;
	childWindowPosPivot.x = 1.0f;
	childWindowPosPivot.y = 0.0f;
	ImGui::SetNextWindowPos(childWindowPos, ImGuiCond_Always, childWindowPosPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::Begin("Scene Overlay", NULL, overlay_window_flags);

	if (!mouseIsCaptured)
		ImGui::Text("Press Shift+F12 to capture mouse");
	else
		ImGui::Text("Press Shift+F12 to release mouse");

	ImGui::End();

	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
			spdlog::info("fire ray to find object to select");
		}
	}

	ImGui::End();
}