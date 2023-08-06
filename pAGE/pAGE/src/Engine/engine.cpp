#include "engine.h"

#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"

#include "Systems/RenderSystem.h"

#include "Components/ComponentUI.h"

float triangleVertices[] = {
	// pos					color
	-0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	0.0f, // left
	0.5f,	-0.5f,	0.0f,	0.0f,	1.0f,	0.0f, // top
	0.0f,	0.5f,	0.0f,	0.0f,	0.0f,	1.0f, // right
};

void ShowInspector(entt::registry& registry, std::vector<Object>& objects, const int& selected);
void ShowSceneHierarchy(std::vector<Object> objects, int& selected);
void ShowScene(ImTextureID texture);

Engine::Engine() {
	isRunning = false;
	windowWidth = 1920;
	windowHeight = 1080;
	selected = -1;
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

	Shader basic2DShader("Basic/basic2D.vert", "Basic/basic2D.frag");
	for (int i = 0; i < 1; ++i) {
		auto triangle = registry.create();
		Object object(triangle);
		object.name = "triangle";
		registry.emplace<TransformComponent>(triangle, glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 90.0f * (i % 3)));
		registry.emplace<ModelComponent>(triangle, triangleVAO, basic2DShader);
		objects.push_back(object);
	}

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
	glViewport(0, 0, 1200, 720);
	// render to framebuffer to create texture for scene viewport
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(.2f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderSystem(registry);

	glViewport(0, 0, windowWidth, windowHeight);
	// render main UI
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ShowSceneHierarchy(objects, selected);
	ShowInspector(registry, objects, selected);
	ShowScene((ImTextureID)texColorBuffer);

	ImGui::Render(); 
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}

void ShowInspector(entt::registry& registry, std::vector<Object>& objects, const int& selected) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowPos(ImVec2(1578, 10));
	ImGui::SetNextWindowSize(ImVec2(332, 755));

	ImGui::Begin("Inspector", NULL, window_flags);

	if (selected != -1) {
		Object& object = objects[selected];
		
		std::string title = "Object : " + object.name;
		ImGui::Text(title.c_str());
		ImGui::Separator();
		ImGui::InputText("name", &object.name);

		if (registry.all_of<TransformComponent>(object.entity)) {
			ComponentUI::PopulateTransformComponent(registry.get<TransformComponent>(object.entity));
		}
		if (registry.all_of<ModelComponent>(object.entity)) {
			ComponentUI::PopulateModelComponent(registry.get<ModelComponent>(object.entity));
		}
	}

	ImGui::End();
}

void ShowSceneHierarchy(std::vector<Object> objects, int& selected) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(332, 755));

	ImGui::Begin("Scene Hierarchy", NULL, window_flags);

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

void ShowScene(ImTextureID texture) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::SetNextWindowPos(ImVec2(352, 10));
	ImGui::SetNextWindowSize(ImVec2(1216, 755));

	ImGui::Begin("Scene", NULL, window_flags);

	ImGui::Image(texture, ImVec2(1200, 720), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
			spdlog::info("Capture mouse now");
		}
	}
	
	ImGui::End();
}