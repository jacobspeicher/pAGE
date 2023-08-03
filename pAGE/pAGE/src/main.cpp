#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <SDL.h>
#include <spdlog/spdlog.h>

#include "Launcher/launcher.h"

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;

int main(int argc, char* argv[])
{
    Launcher launcher;

    launcher.Initialize();
    launcher.Run();
    launcher.Destroy();

    return 0;
}