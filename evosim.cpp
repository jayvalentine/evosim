#include <SDL2/SDL.h>

#include <vector>
#include <utility>
#include <random>

#include <chrono>
#include <thread>

#include <iostream>
#include <fstream>

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

#define START_CREATURES 250
#define MIN_CREATURES 10
#define MAX_CREATURES 5000

// Limit to 40 hrs of runtime.
#define MAX_TIME ((unsigned long)(40 * 60))

// Frames per second of the application.
#define FPS 30

// Global random engine.
std::default_random_engine g_generator;

void PrettyTime(char * buf, unsigned long time)
{
    unsigned int hours = time / 3600;
    unsigned int minutes = (time % 3600) / 60;
    unsigned int seconds = time % 60;

    sprintf(buf, "%02uh %02um %02us", hours, minutes, seconds);
}

bool parse_args(char * argv[], int argc, bool * interactive, unsigned long * pretime, unsigned long * time)
{
    for (int i = 0; i < argc; i++)
    {
        std::string s = std::string(argv[i]);
        if (s == "--interactive" || s == "-i") *interactive = true;
        else if (s == "--pretime" || s == "-p")
        {
            if (i == argc)
            {
                printf("Expected a value after -p/--pretime flag\n");
                return false;
            }

            std::string s_pretime = std::string(argv[i+1]);
            *pretime = std::strtoul(s_pretime.c_str(), NULL, 10);
        }
        else if (s == "--time" || s == "-t")
        {
            if (i == argc)
            {
                printf("Expected a value after -t/--time flag\n");
                return false;
            }

            std::string s_time = std::string(argv[i+1]);
            *time = std::strtoul(s_time.c_str(), NULL, 10);
        }
    }

    return true;
}

int main(int argc, char * argv[])
{
    bool interactive = false;
    unsigned long pretime = 0;
    unsigned long time = MAX_TIME;

    if (!parse_args(argv, argc, &interactive, &pretime, &time))
    {
        printf("Usage error; see previous messages.\n");
        return 1;
    }

    // Limit to MAX_TIME
    time = std::min(time, MAX_TIME);

    SDL_Window * window = NULL;
    SDL_Window * netWindow = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0)
    {
        printf("Could not initialize TTF\n");
        return 1;
    }

    // We've successfully initialized SDL and TTF.

    if (interactive)
    {
        // Now we'll draw the main window.
        window = SDL_CreateWindow("EvoSim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        // This is a window to show the neural network of the selected creature.
        // It's hidden initially.
        netWindow = SDL_CreateWindow("Selected Creature", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 900, SDL_WINDOW_HIDDEN);
    }

    SDL_Event e;

    // Set up the simulation.

    // Initialize a world.
    World * world = new World(WORLD_WIDTH, WORLD_HEIGHT, WORLD_TILESIZE);

    Simulation * sim = new Simulation(world, MIN_CREATURES, FPS);

    // Initialize the random generator.
    Random::Init(std::chrono::system_clock::now().time_since_epoch().count());

    // Initialize creatures.
    for (int i = 0; i < START_CREATURES; i++)
    {
        double x = Random::Double(0, WORLD_WIDTH);
        double y = Random::Double(0, WORLD_HEIGHT);

        sim->AddInitialCreature(Point(x, y));
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

    View * view = NULL;

    if (interactive)
    {
        // Construct a view.
        view = new View(window, netWindow, sim, cameraX, cameraY, cameraScale);
    }
    
    bool quit = false;

    printf("View initialized\n");

    // Store the start time of the simulation.
    unsigned int simulationBegin = SDL_GetTicks();

    // Whether or not we are rendering the view.
    bool renderFlag = true;

    // Pre-render simulation time in seconds.
    unsigned long pretimeSeconds = pretime * 60;
    unsigned long timeSeconds = time * 60;

    try
    {
        // Main loop. Loop until the user quits.
        while (!quit)
        {
            bool render = (interactive && renderFlag && (sim->SimulationTime() >= pretimeSeconds));

            // Get current time (in milliseconds).
            // We'll use this to cap the framerate.
            unsigned int startTime = SDL_GetTicks();

            while (SDL_PollEvent(&e) != 0 && interactive)
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
                    printf("Keydown Event: %d\n", (int)e.key.keysym.sym);

                    switch (e.key.keysym.sym)
                    {
                        case SDLK_DOWN:
                            printf("Pan down\n");
                            view->PanDown();
                            break;
                        case SDLK_UP:
                            printf("Pan up\n");
                            view->PanUp();
                            break;
                        case SDLK_LEFT:
                            printf("Pan left\n");
                            view->PanLeft();
                            break;
                        case SDLK_RIGHT:
                            printf("Pan right\n");
                            view->PanRight();
                            break;
                        case SDLK_EQUALS:
                            printf("Zoom in\n");
                            view->ZoomIn();
                            break;
                        case SDLK_MINUS:
                            printf("Zoom out\n");
                            view->ZoomOut();
                            break;
                        case SDLK_SPACE:
                            printf("Select first creature\n");
                            view->SelectFirst();
                            break;
                        case SDLK_BACKSPACE:
                            printf("Select last creature\n");
                            view->SelectLast();
                            break;
                        case SDLK_r:
                            printf("Toggle rendering\n");
                            renderFlag = !renderFlag;
                            break;
                        case SDLK_1:
                            printf("Select latest generation creature\n");
                            view->SelectHighestGeneration();
                            break;
                        case SDLK_COMMA:
                            printf("Select previous creature\n");
                            view->SelectPrevious();
                            break;
                        case SDLK_PERIOD:
                            printf("Select next creature\n");
                            view->SelectNext();
                            break;
                    }

                    printf("X: %.2f Y: %.2f\n", view->XCoordinate(), view->YCoordinate());
                }

                else if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    if (e.button.button == SDL_BUTTON_LEFT)
                    {
                        printf("Left click event\n");
                        view->HandleClick(e.button.x, e.button.y);
                    }
                }
            }

            // Perform a step of the simulation.
            sim->Step();

            // Render the view to the user.
            if (render)
            {
                view->Render(); 
            }

            // Get the current time (in milliseconds).
            unsigned int endTime = SDL_GetTicks();

            // Work out the time since the simulation started.
            unsigned int totalTime = endTime - simulationBegin;
            
            char runTimeString[100];
            char simTimeString[100];

            PrettyTime(runTimeString, totalTime / 1000);
            PrettyTime(simTimeString, sim->SimulationTime());

            const char * fmt = "Run time: %s, Simulation time: %s, Population: %10lu\n";

            if (render)
            {
                char infoBuf[200];
                sprintf(infoBuf, fmt, runTimeString, simTimeString, sim->CreatureCount());
                view->RenderSimulationInfo(infoBuf);
            }
            else if ((sim->Steps() % (60 * FPS)) == 0)
            {
                printf(fmt, runTimeString, simTimeString, sim->CreatureCount());
            }

            // Work out the time it took to perform this iteration.
            unsigned int elapsedTime = endTime - startTime;

            // We want to make sure that we run the iteration at most 60 times per second.
            // Therefore, each iteration should take 1/60 seconds.
            // Because we're working in milliseconds, this becomes 1000/60.
            //
            // We skip this if we're not rendering.
            if (render && (elapsedTime < (1000.0 / FPS)))
            {
                unsigned int paddingTime = (1000.0 / FPS) - elapsedTime;

                // Sleep for the number of milliseconds calculated.
                std::this_thread::sleep_for(std::chrono::milliseconds(paddingTime));
            }

            // Log the world's population.
            // Do this every 1 (simulation) minutes.
            if ((sim->Steps() % (60 * FPS)) == 0)
            {
                std::ofstream logFile;
                logFile.open("population.log", std::ios::out | std::ios_base::app);

                logFile << sim->CreatureCount() << ":";

                for (int i = 0; i < sim->CreatureCount(); i++)
                {
                    std::shared_ptr<Creature> c = sim->GetCreature(i);

                    logFile << "(" << c->Red() << "," << c->Green() << "," << c->Blue() << "):";
                }

                logFile << std::endl;
            }

            if (sim->CreatureCount() > MAX_CREATURES || sim->SimulationTime() > timeSeconds)
            {
                printf("Maximum population/time exceeded. Exiting...\n");
                quit = true;
            }
        }
    }
    catch (const std::bad_alloc& e)
    {
        printf("Aborted due to bad allocation:\n");
        printf("%s\n", e.what());
    }

    // Delete the view.
    if (view != NULL) delete view;

    // Clean up the window and quit.
    if (interactive) SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();

    return 0;
}
