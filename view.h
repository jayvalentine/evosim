#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>

#include <vector>

#include "simulation.h"
#include "creature.h"

// Camera scrolling constants.
#define CAMERA_ZOOM_FACTOR 2.0
#define CAMERA_SCROLL_FACTOR CAMERA_MAX_SCALE

#define CAMERA_MIN_SCALE 0.2
#define CAMERA_MAX_SCALE 2.0

class View
{
    public:

    // Constructor.
    View(SDL_Window * window, Simulation * sim, double initialX, double initialY, double initialScale);

    // Destructor.
    ~View();

    void Render(void);

    void ZoomIn(void);
    void ZoomOut(void);

    void PanLeft(void);
    void PanRight(void);
    void PanUp(void);
    void PanDown(void);

    void HandleClick(int x, int y);

    void DrawCreature(SDL_Renderer * renderer, Creature * creature, double cameraLeft, double cameraTop);

    private:

    void DrawCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius, unsigned int red, unsigned int green, unsigned int blue);
    void FillCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius, unsigned int red, unsigned int green, unsigned int blue);

    double PanSpeed(void) { return CAMERA_SCROLL_FACTOR / cameraScale; }

    SDL_Renderer * renderer;

    // Simulation object.
    Simulation * simReference;

    // Camera position and scale.
    double cameraX;
    double cameraY;
    double cameraScale;

    // Creature we're focusing on.
    Creature * focusCreature;
};

#endif // VIEW_H
