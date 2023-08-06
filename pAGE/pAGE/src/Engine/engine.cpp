#include "engine.h"

#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"

#include "Systems/RenderSystem.h"

#include "Components/ComponentUI.h"

float triangleVertices[] = {
	// pos					color
	-1.0f,	0.5f,	0.0f,	1.0f,	0.0f,	0.0f, // left
	-0.5f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f, // top
	0.0f,	0.5f,	0.0f,	0.0f,	0.0f,	1.0f, // right
};

void ShowInspector(entt::registry& registry, Object& object);

Engine::Engine() {
	isRunning = false;
	windowWidth = 1200;
	windowHeight = 720;
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
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
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
}

void Engine::Destroy() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	spdlog::info("Engine cleaned up");
}

void Engine::Setup() {
	unsigned int triangleVAO;
	unsigned int triangleVBO;

	glGenVertexArrays(1, &triangleVAO);
	glGenBuffers(1, &triangleVBO);

	// bind triangle object
	glBindVertexArray(triangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);

	// copy vertex data into the VAO
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	// interpret the vertex data for positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	// enable the vertex attrib with the vertex attrib location
	glEnableVertexAttribArray(0);

	// interpret the vertex data for colors and enable
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader basicShader("Basic/basic.vert", "Basic/basic.frag");
	for (int i = 0; i < 1; ++i) {
		auto triangle = registry.create();
		Object object(triangle);
		object.name = "triangle";
		registry.emplace<TransformComponent>(triangle, glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 90.0f * (i % 3)));
		registry.emplace<ModelComponent>(triangle, triangleVAO, basicShader);
		objects.push_back(object);
	}
}

void Engine::Run() {
	while (isRunning) {
		ProcessInput();
		Render();
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
		}
	}
}

void Engine::Render() {
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	ShowInspector(registry, objects[0]);

	RenderSystem(registry);

	ImGui::Render(); 
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}

void ShowInspector(entt::registry& registry, Object& object) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(object.name.c_str(), NULL, window_flags);

	if (registry.all_of<TransformComponent>(object.entity)) {
		ComponentUI::PopulateTransformComponent(registry.get<TransformComponent>(object.entity));
	}
	if (registry.all_of<ModelComponent>(object.entity)) {
		ComponentUI::PopulateModelComponent(registry.get<ModelComponent>(object.entity));
	}

	ImGui::End();
}