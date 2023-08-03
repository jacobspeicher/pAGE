#include <string>

#include "Launcher/launcher.h"

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;

int main(int argc, char* argv[])
{
    std::string workingDirectory = "";

    Launcher launcher;

    launcher.Initialize();
    launcher.Run();
    launcher.Destroy();

    return 0;
}