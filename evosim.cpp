#include <SDL2/SDL.h>

#include <vector>
#include <utility>
#include <random>

// Width and height of main game window, in pixels.
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Width and height of simulated world, in meters.
#define WORLD_WIDTH 1000.0
#define WORLD_HEIGHT 1000.0

#define MIN_SIZE 0.1
#define MAX_SIZE 100.0

#define NUM_CREATURES 1000

void DrawCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

    const int diameter = (radius * 2);

    int x = (radius - 1);
    int y = 0;
    int tx = 1;
    int ty = 1;
    int error = (tx - diameter);

    while (x >= y)
    {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

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

    bool quit = false;

    SDL_Event e;

    // Set up the simulation.

    // A vector of pairs, representing the positions of creatures in the world.
    std::vector<std::pair<double, double>> positions = std::vector<std::pair<double, double>>();

    // A vector of sizes, representing sizes of creatures in the world.
    std::vector<double> sizes = std::vector<double>();

    std::default_random_engine generator;

    // Initialize creatures.
    for (int i = 0; i < NUM_CREATURES; i++)
    {
        std::uniform_real_distribution<double> xDistribution(0, WORLD_WIDTH);
        std::uniform_real_distribution<double> yDistribution(0, WORLD_WIDTH);
        std::uniform_real_distribution<double> sizeDistribution(MIN_SIZE, MAX_SIZE);

        double x = xDistribution(generator);
        double y = yDistribution(generator);
        double size = sizeDistribution(generator);

        positions.push_back(std::pair<double, double>(x, y));
        sizes.push_back(size);
    }

    // Initialize the camera position.
    // This is the center of the camera view in the simulated world.
    double cameraX = WORLD_WIDTH / 2;
    double cameraY = WORLD_HEIGHT / 2;

    // Initialize the camera scale.
    // This is the number of pixels per metre.
    // i.e., scale=1 means 1m is 1p, scale=45 means 1m is 45p.
    double cameraScale = 20.0;

    // Get a renderer for the window.
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderClear(renderer);

    // Determine the width and height (in metres) of the camera.
    double cameraWidth = SCREEN_WIDTH / cameraScale;
    double cameraHeight = SCREEN_HEIGHT / cameraScale;

    // Now we can determine the boundaries of the camera in metres, and so
    // work out what we can display.
    double cameraLeft = cameraX - (cameraWidth / 2);
    double cameraRight = cameraX + (cameraWidth / 2);

    double cameraTop = cameraY - (cameraHeight / 2);
    double cameraBottom = cameraY + (cameraHeight / 2);

    // Iterate over the creatures and draw a pixel for each one.
    for (int i = 0; i < NUM_CREATURES; i++)
    {
        double creatureX = positions[i].first;
        double creatureY = positions[i].second;

        double creatureSize = sizes[i];
        
        if (creatureX < cameraLeft) continue;
        if (creatureX > cameraRight) continue;
        
        if (creatureY < cameraTop) continue;
        if (creatureY > cameraBottom) continue;

        // If we get here then the creature is within the bounds of the camera,
        // so lets calculate its pixel position.

        double creatureWithinX = creatureX - cameraLeft;
        double creatureWithinY = creatureY - cameraTop;

        // Scale both to get the pixel position.
        int creaturePixelX = (int) (creatureWithinX * cameraScale);
        int creaturePixelY = (int) (creatureWithinY * cameraScale);
        int creaturePixelRadius = (int) ((creatureSize / 2) * cameraScale);

        // Now draw a circle!
        DrawCircle(renderer, creaturePixelX, creaturePixelY, creaturePixelRadius);
    }

    SDL_RenderPresent(renderer);

    // Main loop. Loop until the user quits.
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT) quit = true;
        }
    }

    // Clean up the window and quit.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
