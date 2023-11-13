#include "Launcher.h"

Launcher::Launcher() {
	isRunning = false;
	windowWidth = 800;
	windowHeight = 600;
	spdlog::info("{0} Created", outputPrefix);
}

Launcher::~Launcher() {
	spdlog::info("{0} Destroyed", outputPrefix);
}

void Launcher::Initialize(std::shared_ptr<EventBus>& eventBus) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		spdlog::error("{0} SDL could not be initialized", outputPrefix);
		return;
	}

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
		spdlog::error("{0} SDL Window could not be created", outputPrefix);
		return;
	}

	// create SDL renderer
	// -1 in arg[1] tells renderer to use the first available rendering driver
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		spdlog::error("{0} SDL Renderer could not be created", outputPrefix);
		return;
	}

	// initialize imgui
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGuiFileBrowserFlags filebrowser_flags = ImGuiFileBrowserFlags_SelectDirectory |
		ImGuiFileBrowserFlags_CreateNewDir;
	fileDialog = std::make_unique<ImGui::FileBrowser>(filebrowser_flags);

	isRunning = true;

	// check if project list json file exists, if not, create it
	FILE* projectFile;
	errno_t err = fopen_s(&projectFile, "projects.lst", "rb");
	if (err != 0) {
		FILE* newProjectFile;
		err = fopen_s(&newProjectFile, "projects.lst", "wb");
		if (newProjectFile) {
			fprintf(newProjectFile, "{}");
			fclose(newProjectFile);
		}
	}

	this->eventBus = eventBus;
	
	if (projectFile) {
		fclose(projectFile);
	}
}

void Launcher::Destroy() {
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	spdlog::info("{0} Cleaned up", outputPrefix);
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

	ShowActionsWindow();
	ShowProjectsWindow();

	//ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

	// display what was rendererd in the window
	// swaps the back buffer and the front buffer
	// everything is rendered in the back buffer and then the front & back buffers are
	//   swapped so everything is drawn at once
	SDL_RenderPresent(renderer);
}

void Launcher::ShowActionsWindow() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(780, 35), ImGuiCond_FirstUseEver);
	ImGui::Begin("pAGE Launcher Actions", (bool*)true, window_flags);

	if (ImGui::Button("New Project")) {
		ImGui::OpenPopup("Create New Project");
	}
	ImGui::SameLine();
	if (selected == -1) {
		ImGui::BeginDisabled(true);
	}
	if (ImGui::Button("Open Project")) {
		eventBus->EmitEvent<ProjectLoadedEvent>(selectedProject);
		isRunning = false;
	}
	if (selected == -1) {
		ImGui::EndDisabled();
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Create New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		static char name[128] = "";
		ImGui::InputText("Project Name", name, IM_ARRAYSIZE(name));

		if (ImGui::Button("...", ImVec2(50, 0))) {
			fileDialog->Open();
		}
		fileDialog->Display();
		ImGui::SameLine();
		static char path[512] = "";
		if (fileDialog->HasSelected()) {
			strcpy_s(path, fileDialog->GetSelected().string().c_str());
		}
		ImGui::InputText("Project Path", path, IM_ARRAYSIZE(path));

		bool exists = false;
		std::ifstream f(path + std::string("/project.ini"));
		exists = f.good();
		f.close();

		ImGui::Separator();

		if (exists) {
			ImGui::Text("Project already exists at this location");
		}

		if (name[0] == '\0' || path[0] == '\0' || exists) {
			ImGui::BeginDisabled(true);
		}
		bool saved = false;
		if (ImGui::Button("Create", ImVec2(120, 0))) {
			std::ofstream projectListFile("projects.lst", std::ios_base::app);
			if (projectListFile.is_open()) {
				projectListFile << name << "," << path << "\n";
				projectListFile.close();

				std::string projectIniPath = path + std::string("/project.ini");
				std::ofstream projectIniFile(projectIniPath);
				if (projectIniFile.is_open()) {
					projectIniFile << "[pAGE Project]\n";
					projectIniFile << name << "\n";
				}
			}
			else {
				spdlog::error("{0} Could not create project", outputPrefix);
			}

			strcpy_s(name, "");
			strcpy_s(path, "");
			saved = true;
			ImGui::CloseCurrentPopup();
		}
		if ((name[0] == '\0' || path[0] == '\0' || exists) && !saved) {
			ImGui::EndDisabled();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			strcpy_s(name, "");
			strcpy_s(path, "");
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


	ImGui::End();
}

void Launcher::ShowProjectsWindow() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowPos(ImVec2(10, 55), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(780, 535), ImGuiCond_FirstUseEver);
	ImGui::Begin("pAGE Project List", (bool*)true, window_flags);

	std::string line;
	std::ifstream projectFile("projects.lst");
	std::vector<std::string> projectNames;
	std::vector<std::string> projectPaths;
	if (projectFile.is_open()) {
		while (getline(projectFile, line)) {
			projectNames.push_back(line.substr(0, line.find(",")));
			projectPaths.push_back(line.substr(line.find(",") + 1));
		}
	}

	ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV |
		ImGuiTableFlags_RowBg;
	if (ImGui::BeginTable("Project List Table", 2, table_flags)) {
		ImGui::TableSetupColumn("Project Name");
		ImGui::TableSetupColumn("Project Path");
		ImGui::TableHeadersRow();

		static int sel = -1;
		for (int i = 0; i < static_cast<int>(projectNames.size()); ++i) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::Selectable(projectNames[i].c_str(), sel == i, ImGuiSelectableFlags_SpanAllColumns)) {
				sel = i;
				selectedProject.name = projectNames[i];
				selectedProject.path = projectPaths[i];
			}
			ImGui::TableNextColumn();
			ImGui::Text(projectPaths[i].c_str());
		}
		selected = sel;
		ImGui::EndTable();
	}

	ImGui::End();
}