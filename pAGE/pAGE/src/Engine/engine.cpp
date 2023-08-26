#include "engine.h"

#include "Objects/Sprite.h"

#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ComponentUI.h"

#include "Systems/RenderSystem.h"
#include "Systems/RenderDebugBoxSystem.h"
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
	selected = -1;
	mouseIsCaptured = false;

	/* Debug */
	debug = false;

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

	spdlog::info("Engine cleaned up");
}

void Engine::Setup() {
	OpenGLObjectsLoader::LoadOpenGLObjects(assetStore);
	ModelLoader::LoadModels(assetStore);
	ShaderLoader::LoadShaders(assetStore);
	TextureLoader::LoadTextures(assetStore);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture to use as color attachment 0 for framebuffer
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach texture to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// generate texture to use as color attachment 1 for framebuffer
	glGenTextures(1, &selColorBuffer);
	glBindTexture(GL_TEXTURE_2D, selColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
			if (sdlEvent.key.keysym.sym == SDLK_BACKQUOTE)
				debug = !debug;
			break;
		case SDL_MOUSEBUTTONUP:
			if (mouseIsCaptured && sdlEvent.button.button == 3) {
				mouseIsCaptured = false;
				SDL_SetRelativeMouseMode((SDL_bool)false);
				SDL_WarpMouseInWindow(window, windowWidth / 2, windowHeight / 2);
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
	glClearColor(.2f, .2f, .2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderSystem(registry, camera);
	if (debug)
		RenderDebugBoxSystem(registry, camera, assetStore);

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
	if (debug)
		ShowScene((ImTextureID)selColorBuffer);
	else
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
		if (registry.all_of<DirectionalLightComponent>(object.entity)) {
			ComponentUI::PopulateDirectionalLightComponent(registry.get<DirectionalLightComponent>(object.entity));
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
				objects.push_back(object);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("3D")) {
			ImGui::SeparatorText("Basic");
			if (ImGui::MenuItem("Cube")) {
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto cube = registry.create();
				Object object(cube);
				object.name = "Cube (" + std::to_string((long)cube) + ")";
				registry.emplace<TransformComponent>(cube, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(cube, assetStore.GetModel("cube"), shader);
				objects.push_back(object);
			}
			if (ImGui::MenuItem("Sphere")) {
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto sphere = registry.create();
				Object object(sphere);
				object.name = "Sphere (" + std::to_string((long)sphere) + ")";
				registry.emplace<TransformComponent>(sphere, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(sphere, assetStore.GetModel("sphere"), shader);
				objects.push_back(object);
			}
			if (ImGui::MenuItem("Cylinder")) {
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto cylinder = registry.create();
				Object object(cylinder);
				object.name = "Cylinder (" + std::to_string((long)cylinder) + ")";
				registry.emplace<TransformComponent>(cylinder, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(cylinder, assetStore.GetModel("cylinder"), shader);
				objects.push_back(object);
			}
			if (ImGui::MenuItem("Cone")) {
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto cone = registry.create();
				Object object(cone);
				object.name = "Cone (" + std::to_string((long)cone) + ")";
				registry.emplace<TransformComponent>(cone, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(cone, assetStore.GetModel("cone"), shader);
				objects.push_back(object);
			}

			ImGui::SeparatorText("Custom");
			if (ImGui::MenuItem("Backpack")) {
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto backpack = registry.create();
				Object object(backpack);
				object.name = "Backpack (" + std::to_string((long)backpack) + ")";
				registry.emplace<TransformComponent>(backpack, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(backpack, assetStore.GetModel("backpack"), shader);
				objects.push_back(object);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Lights")) {
			if (ImGui::MenuItem("Directional")) {
				auto directional = registry.create();
				Object object(directional);
				object.name = "Directional (" + std::to_string((long)directional) + ")";
				std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
				registry.emplace<TransformComponent>(directional, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<DirectionalLightComponent>(directional, glm::vec3(0, 0, -1), glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(0.5f));
				registry.emplace<SpriteComponent>(directional, sprite, assetStore.GetTexture("directionalLight"), assetStore.GetShader("basic2D"));
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
		if (ImGui::BeginPopupContextItem()) {
			selected = i;
			if (ImGui::Button("Delete")) {
				registry.destroy(objects[i].entity);
				objects.erase(objects.begin() + i);
				selected = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
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
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoMouseInputs;

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

	float regionMouseX = io->MousePos.x - windowPos.x - 8; // account for image window padding
	float regionMouseY = io->MousePos.y - windowPos.y - 27; // account for image window padding

	ImGui::Text("Camera Position\nx: %.2f, y: %.2f, z: %.2f", camera.Position.x, camera.Position.y, camera.Position.z);
	ImGui::Text("Mouse Position\nx: %.2f, y: %.2f", regionMouseX, regionMouseY);

	ImGui::End();

	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
			glm::vec2 ndc(
				(2.0f * regionMouseX) / 1200.0f - 1.0f,
				1.0f - (2.0f * regionMouseY) / 720.f
			);
			// SelectObjectSystem(registry, camera, ndc, selected);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			unsigned char data[3];
			glReadPixels(regionMouseX, 720 - regionMouseY, (GLint)1, (GLint)1, GL_RGB, GL_UNSIGNED_BYTE, data);
			if (data[1] == 0) {
				for (int i = 0; i < objects.size(); ++i) {
					if ((long)objects[i].entity == data[0]) {
						selected = i;
						break;
					}
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			mouseIsCaptured = true;
			SDL_SetRelativeMouseMode((SDL_bool)true);
		}
	}

	ImGui::End();
}