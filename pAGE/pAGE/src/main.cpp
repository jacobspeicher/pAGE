#include <SDL.h>
#include <entt/entt.hpp>

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;

//The parameters in the main function cannot be omitted, or an error will be reported
int main(int arg, char* argv[])
{
    window = SDL_CreateWindow("SDL2 Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window)
        return -1;
    surface = SDL_GetWindowSurface(window);
    if (!surface)
        return -2;

    bool isRunning = true;

    SDL_Rect rec;
    rec.x = 700;
    rec.y = 10,
        rec.w = 20;
    rec.h = 20;

    while (isRunning)
    {
        SDL_FillRect(surface, &rec, SDL_MapRGB(surface->format, 180, 10, 140));

        rec.x += 6;
        rec.y += 2;
        rec.x = rec.x > 800 ? 0 : rec.x;
        rec.y = rec.y > 600 ? 0 : rec.y;

        SDL_FillRect(surface, &rec, SDL_MapRGB(surface->format, 10, 200, 120));

        SDL_UpdateWindowSurface(window);
        SDL_Delay((1.0 / 60) * 1000);

        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
            }
        }
    }

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}