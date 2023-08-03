#include "launcher.h"

Launcher::Launcher() {
	isRunning = false;
	spdlog::info("Launcher created");
}

Launcher::~Launcher() {
	spdlog::info("Launcher destroyed");
}

void Launcher::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		spdlog::error("SDL could not be initialized");
		return;
	}

	windowWidth = 800;
	windowHeight = 600;

	// create SDL window
	window = SDL_CreateWindow(
		"pAGE Launcher",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN
	);
	if (!window) {
		spdlog::error("SDL Window could not be created");
		return;
	}

	// create SDL renderer
	// -1 in arg[1] tells renderer to use the first available rendering driver
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		spdlog::error("SDL Renderer could not be created");
		return;
	}

	// initialize imgui
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	isRunning = true;
}

void Launcher::Destroy() {
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	spdlog::info("Launcher cleaned up");
}

void Launcher::Setup() {

}

void Launcher::Run() {
	while (isRunning) {
		ProcessInput();
		Render();
	}
}

void Launcher::ProcessInput() {
	SDL_Event sdlEvent;

	// process every SDL_Event that SDL_PollEvent captures
	while (SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		switch (sdlEvent.type) {
			// SDL_QUIT is generated when clicking the 'X' button for the operating system
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

void Launcher::Render() {
	// set the background color of the whole window, used to clear window every render call
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

	// display what was rendererd in the window
	// swaps the back buffer and the front buffer
	// everything is rendered in the back buffer and then the front & back buffers are
	//   swapped so everything is drawn at once
	SDL_RenderPresent(renderer);
}