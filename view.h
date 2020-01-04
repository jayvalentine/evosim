#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>

#include <vector>

#include "world.h"
#include "creature.h"

// Camera scrolling constants.
#define CAMERA_ZOOM_FACTOR 2.0
#define CAMERA_SCROLL_FACTOR CAMERA_MAX_SCALE

#define CAMERA_MIN_SCALE 0.1
#define CAMERA_MAX_SCALE 50.0

class View
{
    public:

    // Constructor.
    View(SDL_Window * window, World * world, double initialX, double initialY, double initialScale);

    // Destructor.
    ~View();

    void Render(void);

    void ZoomIn(void);
    void ZoomOut(void);

    void PanLeft(void);
    void PanRight(void);
    void PanUp(void);
    void PanDown(void);

    private:

    void DrawCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius, unsigned int red, unsigned int green, unsigned int blue);

    double PanSpeed(void) { return CAMERA_SCROLL_FACTOR / cameraScale; }

    SDL_Renderer * renderer;

    // World object.
    World * worldReference;

    // Camera position and scale.
    double cameraX;
    double cameraY;
    double cameraScale;
};

#endif // VIEW_H
