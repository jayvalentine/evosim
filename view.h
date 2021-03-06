#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <memory>

#include "simulation.h"
#include "creature.h"
#include "world.h"

// Camera scrolling constants.
#define CAMERA_ZOOM_FACTOR 2.0
#define CAMERA_SCROLL_FACTOR (CAMERA_MAX_SCALE * 20)

#define CAMERA_MIN_SCALE 0.2
#define CAMERA_MAX_SCALE 2.0

class View
{
    public:

    // Constructor.
    View(SDL_Window * window, SDL_Window * netWindow, Simulation * sim, double initialX, double initialY, double initialScale);

    // Destructor.
    ~View();

    void Render(void);

    void RenderSimulationInfo(const char * buf);

    void ZoomIn(void);
    void ZoomOut(void);

    void PanLeft(void);
    void PanRight(void);
    void PanUp(void);
    void PanDown(void);

    double XCoordinate(void);
    double YCoordinate(void);

    double CameraLeft(void);
    double CameraRight(void);
    double CameraTop(void);
    double CameraBottom(void);

    void SelectFirst(void);
    void SelectLast(void);
    void SelectHighestGeneration(void);
    void SelectPrevious(void);
    void SelectNext(void);

    void HandleClick(int x, int y);

    void DrawCreature(SDL_Renderer * renderer, Creature * creature, double cameraLeft, double cameraTop);

    private:

    void RenderText(SDL_Renderer * r, const char * text, int x, int y);

    void RenderNet(NeuralNetwork * net);
    void RenderInfo(Creature * creature);

    void DrawCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius, unsigned int red, unsigned int green, unsigned int blue);
    void FillCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius, unsigned int red, unsigned int green, unsigned int blue);

    double PanSpeed(void) { return CAMERA_SCROLL_FACTOR / cameraScale; }

    SDL_Renderer * renderer;
    SDL_Renderer * netRenderer;

    SDL_Window * netWindowReference;

    // Simulation object.
    Simulation * simReference;

    // Camera position and scale.
    Point cameraPosition;
    double cameraScale;

    // Creature we're focusing on.
    std::shared_ptr<Creature> focusCreature;
    int focusIndex;

    // Font used throughout rendering.
    TTF_Font * font;
};

#endif // VIEW_H
