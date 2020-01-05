#include <SDL2/SDL.h>

#include <vector>
#include <utility>
#include <random>

#include <chrono>
#include <thread>

#include "world.h"
#include "creature.h"
#include "view.h"

// Width and height of main game window, in pixels.
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Width and height of simulated world, in meters.
#define WORLD_WIDTH 10000.0
#define WORLD_HEIGHT 10000.0
#define WORLD_TILESIZE 100.0

// Minimum and maximum size of creatures.
#define MIN_SIZE 0.1
#define MAX_SIZE 100.0

#define NUM_CREATURES 1000

// Frames per second of the application.
#define FPS 60

// Global random engine.
std::default_random_engine g_generator;

int main(int argc, char * argv[])
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

    SDL_Event e;

    // Set up the simulation.

    // Initialize a world.
    World * world = new World(WORLD_WIDTH, WORLD_HEIGHT, WORLD_TILESIZE);

    // Initialize the random generator.
    Random::Init(std::chrono::system_clock::now().time_since_epoch().count());

    // Initialize creatures.
    for (int i = 0; i < NUM_CREATURES; i++)
    {
        double x = Random::Double(0, WORLD_WIDTH);
        double y = Random::Double(0, WORLD_HEIGHT);
        double size = Random::Double(MIN_SIZE, MAX_SIZE);

        world->AddCreature(x, y, size);
    }

    printf("World initialized\n");

    // Initialize the camera position.
    // This is the center of the camera view in the simulated world.
    const double cameraX = WORLD_WIDTH / 2;
    const double cameraY = WORLD_HEIGHT / 2;

    // Initialize the camera scale.
    // This is the number of pixels per metre.
    // i.e., scale=1 means 1m is 1p, scale=45 means 1m is 45p.
    const double cameraScale = 1.0;

    // Construct a view.
    View * view = new View(window, world, cameraX, cameraY, cameraScale);
    
    bool quit = false;

    printf("View initialized\n");

    // Main loop. Loop until the user quits.
    while (!quit)
    {

        // Get current time (in milliseconds).
        // We'll use this to cap the framerate.
        unsigned int startTime = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                printf("Exiting...\n");
                quit = true;
            }

            else if (e.type == SDL_MOUSEWHEEL)
            {
                printf("Scroll Event\n");

                if (e.wheel.y > 0) view->ZoomIn();
                else if (e.wheel.y < 0) view->ZoomOut();
            }

            else if (e.type == SDL_KEYDOWN)
            {
                printf("Keydown Event\n");

                switch (e.key.keysym.sym)
                {
                    case SDLK_DOWN:
                        view->PanDown();
                        break;
                    case SDLK_UP:
                        view->PanUp();
                        break;
                    case SDLK_LEFT:
                        view->PanLeft();
                        break;
                    case SDLK_RIGHT:
                        view->PanRight();
                        break;
                    case SDLK_EQUALS:
                        view->ZoomIn();
                        break;
                    case SDLK_MINUS:
                        view->ZoomOut();
                        break;
                }
            }

            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    view->HandleClick(e.button.x, e.button.y);
                }
            }
        }

        // Perform a step of the world.
        world->Step();

        // Render the view to the user.
        view->Render(); 

        // Get the current time (in milliseconds), and work out the time it took to perform this iteration.
        unsigned int endTime = SDL_GetTicks();

        unsigned int elapsedTime = endTime - startTime;

        // We want to make sure that we run the iteration at most 60 times per second.
        // Therefore, each iteration should take 1/60 seconds.
        // Because we're working in milliseconds, this becomes 1000/60.
        if (elapsedTime < (1000 / 60))
        {
            unsigned int paddingTime = (1000 / 60) - elapsedTime;

            // Sleep for the number of milliseconds calculated.
            std::this_thread::sleep_for(std::chrono::milliseconds(paddingTime));
        }
    }

    // Delete the view.
    delete view;

    // Clean up the window and quit.
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
