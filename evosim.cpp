#include <SDL2/SDL.h>

// Width and height of main game window.
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int main(int argc, char* argv[])
{
    SDL_Window * window = NULL;

    SDL_Surface * screenSurface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // We've successfully initialized SDL.

    // Now we'll draw the main window.
    window = SDL_CreateWindow("EvoSim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    bool quit = false;

    SDL_Event e;
    
    // Main loop. Loop until the user quits.
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT) quit = true;
        }
    }

    // Clean up the window and quit.
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
