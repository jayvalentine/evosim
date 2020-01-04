#include "view.h"

View::View(SDL_Window * window, World * world, double initialX, double initialY, double initialScale)
{
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    worldReference = world;

    cameraX = initialX;
    cameraY = initialY;
    cameraScale = initialScale;
}

View::~View()
{
    SDL_DestroyRenderer(renderer);
}

void View::Render(void)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    int width;
    int height;

    SDL_GetRendererOutputSize(renderer, &width, &height);

    // Determine the width and height (in metres) of the camera.
    double cameraWidth = width / cameraScale;
    double cameraHeight = height / cameraScale;

    // Now we can determine the boundaries of the camera in metres, and so
    // work out what we can display.
    double cameraLeft = cameraX - (cameraWidth / 2);
    double cameraRight = cameraX + (cameraWidth / 2);

    double cameraTop = cameraY - (cameraHeight / 2);
    double cameraBottom = cameraY + (cameraHeight / 2);

    // Draw the tiles for the world.
    for (int x = 0; x < width; x++)
    {
        double realX = (((double) x) / cameraScale) + cameraLeft;

        int y = 0;
        while (y < height)
        {
            // Calculate the real-world position of this pixel.    
            double realY = (((double) y) / cameraScale) + cameraTop;

            // Caclulate the top boundary of the tile.
            double tileTop = ((int) realY / worldReference->TileSize()) * worldReference->TileSize();

            // We can now calculate the bottom boundary of the tile (in pixels)
            double tileBottom = tileTop + (worldReference->TileSize());

            int tileBottomPixels = (int) (tileBottom - cameraTop) * cameraScale;

            if (tileBottomPixels >= height) tileBottomPixels = height - 1;

            double tileValue = worldReference->GetTile(realX, realY);

            unsigned int green = 100 + (int)((tileValue / 100.0) * 155);

            SDL_SetRenderDrawColor(renderer, 100, green, 100, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(renderer, x, y, x, tileBottomPixels);

            // Now move onto the next tile.
            y = tileBottomPixels + 1;
        }
    }

    // Iterate over the creatures and draw a pixel for each one.
    for (int i = 0; i < worldReference->CreatureCount(); i++)
    {
        Creature * creature = worldReference->GetCreature(i);

        double creatureX = creature->GetXPosition();
        double creatureY = creature->GetYPosition();

        double creatureSize = creature->GetSize();
        
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
        // Actually, draw several circles, so that we get a thick edge.
        for (int i = 0; i < 5; i++)
        {
            DrawCircle(renderer, creaturePixelX, creaturePixelY, creaturePixelRadius - i, creature->Red(), creature->Green(), creature->Blue());
        }
    }

    SDL_RenderPresent(renderer);
}

void View::ZoomIn(void)
{
    cameraScale = cameraScale * CAMERA_ZOOM_FACTOR;
    if (cameraScale > CAMERA_MAX_SCALE) cameraScale = CAMERA_MAX_SCALE;
}

void View::ZoomOut(void)
{
    cameraScale = cameraScale / CAMERA_ZOOM_FACTOR;
    if (cameraScale < CAMERA_MIN_SCALE) cameraScale = CAMERA_MIN_SCALE;
}

void View::PanLeft(void)
{
    cameraX -= PanSpeed();
    if (cameraX < 0.0) cameraX = 0.0;
}

void View::PanRight(void)
{
    cameraX += PanSpeed();
    if (cameraX > worldReference->Width()) cameraX = worldReference->Width();
}

void View::PanUp(void)
{
    cameraY -= PanSpeed();
    if (cameraY < 0.0) cameraY = 0.0;
}

void View::PanDown(void)
{
    cameraY += PanSpeed();
    if (cameraY > worldReference->Height()) cameraY = worldReference->Height();
}

void View::DrawCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius, unsigned int red, unsigned int green, unsigned int blue)
{
    SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);

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