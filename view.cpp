#include "view.h"

View::View(SDL_Window * window, SDL_Window * netWindow, Simulation * sim, double initialX, double initialY, double initialScale)
{
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    netWindowReference = netWindow;

    netRenderer = SDL_CreateRenderer(netWindow, -1, SDL_RENDERER_ACCELERATED);

    simReference = sim;

    cameraX = initialX;
    cameraY = initialY;
    cameraScale = initialScale;

    focusCreature = NULL;
}

View::~View()
{
    SDL_DestroyRenderer(renderer);
}

void View::RenderNet(NeuralNetwork * net)
{
    // Render the input neurons.
    std::vector<int> inputs = net->Inputs();

    SDL_SetRenderDrawColor(netRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(netRenderer);

    for (int i = 0; i < inputs.size(); i++)
    {
        int x = 40;
        int y = 40 + (70 * inputs[i]);

        printf("Drawing %d, %d\n", x, y);

        DrawCircle(netRenderer, x, y, 30, 255, 255, 255);

        // Determine the colour of the neuron.
        // Red if the neuron is negative,
        // White if the neuron is 0,
        // Green if the neuron is positive.

        unsigned int red = 255;
        unsigned int green = 255;
        unsigned int blue = 255;

        double value = net->NeuronValue(inputs[i]);

        // We expect value to be betwen -1 and 1, but lets bound it to be sure.
        if (value < -1.0) value = -1.0;
        else if (value > 1.0) value = 1.0;

        if (value < 0.0)
        {
            unsigned int subtract = (unsigned int) (-value * 255);
            green -= subtract;
            blue -= subtract;
        }
        else if (value > 0.0)
        {
            unsigned int subtract = (unsigned int) (value * 255);
            red -= subtract;
            blue -= subtract;
        }

        // Now fill the circle we just drew with the calculated color.
        FillCircle(netRenderer, x, y, 29, red, green, blue);
    }


    SDL_RenderPresent(netRenderer);
}

void View::Render(void)
{
    // If we're focusing on a creature, set the camera position.
    if (focusCreature != NULL)
    {
        cameraX = focusCreature->GetXPosition();
        cameraY = focusCreature->GetYPosition();

        // Show the information window.
        SDL_ShowWindow(netWindowReference);

        // Render the creature's neural net to the information window.
        RenderNet(focusCreature->Net());
    }

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
    if (cameraLeft < 0.0) cameraLeft = 0.0;
    else if (cameraLeft > (simReference->GetWorld()->Width() - cameraWidth)) cameraLeft = simReference->GetWorld()->Width() - cameraWidth;

    double cameraRight = cameraLeft + cameraWidth;

    double cameraTop = cameraY - (cameraHeight / 2);
    if (cameraTop < 0.0) cameraTop = 0.0;
    else if (cameraTop > (simReference->GetWorld()->Height() - cameraHeight)) cameraTop = simReference->GetWorld()->Height() - cameraHeight;

    double cameraBottom = cameraTop + cameraHeight;

    // Draw the tiles for the world.
    int viewX = 0;
    while (viewX < width)
    {
        double realX = (((double) viewX) / cameraScale) + cameraLeft;

        // Calculate the left boundary of the tile.
        double tileLeft = ((int) (realX / simReference->GetWorld()->TileSize())) * simReference->GetWorld()->TileSize();

        // Calculate right boundary of tile in pixels
        double tileRight = tileLeft + simReference->GetWorld()->TileSize();

        int tileRightPixels = (int) (tileRight - cameraLeft) * cameraScale;

        if (tileRightPixels >= width) tileRightPixels = width - 1;

        if (tileRightPixels < viewX)
        {
            viewX++;
            continue;
        }

        int viewY = 0;
        while (viewY < height)
        {
            // Calculate the real-world position of this pixel.    
            double realY = (((double) viewY) / cameraScale) + cameraTop;

            // Caclulate the top boundary of the tile.
            double tileTop = ((int) (realY / simReference->GetWorld()->TileSize())) * simReference->GetWorld()->TileSize();

            // We can now calculate the bottom boundary of the tile (in pixels)
            double tileBottom = tileTop + simReference->GetWorld()->TileSize();

            int tileBottomPixels = (int) (tileBottom - cameraTop) * cameraScale;

            if (tileBottomPixels >= height) tileBottomPixels = height - 1;

            double tileValue = simReference->GetWorld()->GetTile(realX, realY);

            if (tileBottomPixels < viewY) 
            {
                viewY++;
                continue;
            }

            unsigned int green = 100 + (int)((tileValue / 100.0) * 155);

            // Create a rectangle for this tile.
            SDL_Rect tile;

            tile.x = viewX;
            tile.y = viewY;
            tile.w = (tileRightPixels - viewX);
            tile.h = (tileBottomPixels - viewY);

            SDL_SetRenderDrawColor(renderer, 100, green, 100, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &tile);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &tile);

            // Now move onto the next tile.
            
            viewY = tileBottomPixels + 1;
        }

        // Now move onto the next column.
        viewX = tileRightPixels + 1;
    }

    // Iterate over the creatures and draw each one.
    for (int i = 0; i < simReference->CreatureCount(); i++)
    {
        Creature * creature = simReference->GetCreature(i);

        double creatureX = creature->GetXPosition();
        double creatureY = creature->GetYPosition();
        
        if (creatureX < cameraLeft) continue;
        if (creatureX > cameraRight) continue;
        
        if (creatureY < cameraTop) continue;
        if (creatureY > cameraBottom) continue;

        DrawCreature(renderer, creature, cameraLeft, cameraTop);
    }

    SDL_RenderPresent(renderer);
}

void View::ZoomIn(void)
{
    cameraScale = cameraScale * CAMERA_ZOOM_FACTOR;
    if (cameraScale > CAMERA_MAX_SCALE) cameraScale = CAMERA_MAX_SCALE;
    printf("Scale: %f\n", cameraScale);
}

void View::ZoomOut(void)
{
    cameraScale = cameraScale / CAMERA_ZOOM_FACTOR;
    if (cameraScale < CAMERA_MIN_SCALE) cameraScale = CAMERA_MIN_SCALE;
    printf("Scale: %f\n", cameraScale);
}

void View::PanLeft(void)
{
    cameraX -= PanSpeed();
    if (cameraX < 0.0) cameraX = 0.0;
}

void View::PanRight(void)
{
    cameraX += PanSpeed();
    if (cameraX > simReference->GetWorld()->Width()) cameraX = simReference->GetWorld()->Width();
}

void View::PanUp(void)
{
    cameraY -= PanSpeed();
    if (cameraY < 0.0) cameraY = 0.0;
}

void View::PanDown(void)
{
    cameraY += PanSpeed();
    if (cameraY > simReference->GetWorld()->Height()) cameraY = simReference->GetWorld()->Height();
}

void View::HandleClick(int x, int y)
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
    if (cameraLeft < 0.0) cameraLeft = 0.0;
    else if (cameraLeft > (simReference->GetWorld()->Width() - cameraWidth)) cameraLeft = simReference->GetWorld()->Width() - cameraWidth;

    double cameraTop = cameraY - (cameraHeight / 2);
    if (cameraTop < 0.0) cameraTop = 0.0;
    else if (cameraTop > (simReference->GetWorld()->Height() - cameraHeight)) cameraTop = simReference->GetWorld()->Height() - cameraHeight;

    // First, calculate the real-world position of the click.
    double realX = cameraLeft + (x / cameraScale);
    double realY = cameraTop + (y / cameraScale);

    // Is this position a creature?
    for (int i = 0; i < simReference->CreatureCount(); i++)
    {
        Creature * creature = simReference->GetCreature(i);

        // Calculate distance from this creature to the clicked point.
        double xDist = fabs(realX - creature->GetXPosition());
        double yDist = fabs(realY - creature->GetYPosition());

        // A bit of year 7 maths...
        double distance = sqrt((xDist * xDist) + (yDist * yDist));

        // A creature's size is its diameter in metres, so we want to see if
        // distance is within the radius.
        if (distance < (creature->GetSize() / 2))
        {
            // We are now following this creature.
            focusCreature = creature;
            break;
        }
    }
}

void View::DrawCreature(SDL_Renderer * renderer, Creature * creature, double cameraLeft, double cameraTop)
{
    double creatureX = creature->GetXPosition();
    double creatureY = creature->GetYPosition();

    double creatureSize = creature->GetSize();

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
    //
    // Here we define the thickness in terms of metres (2.5m), so that the pixel-width of the line
    // changes as we scale.
    int edgeThickness = (int) (2.5 * cameraScale);
    if (edgeThickness < 1) edgeThickness = 1;

    for (int i = 0; i < edgeThickness; i++)
    {
        DrawCircle(renderer, creaturePixelX, creaturePixelY, creaturePixelRadius - i, 0, 0, 0);
    }

    // Now fill the circle.
    FillCircle(renderer, creaturePixelX, creaturePixelY, creaturePixelRadius - edgeThickness, creature->Red(), creature->Green(), creature->Blue());
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

void View::FillCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius, unsigned int red, unsigned int green, unsigned int blue)
{
	for (double dy = 1; dy <= radius; dy += 1.0)
	{
		// This loop is unrolled a bit, only iterating through half of the
		// height of the circle.  The result is used to draw a scan line and
		// its mirror image below it.

		// The following formula has been simplified from our original.  We
		// are using half of the width of the circle because we are provided
		// with a center and we need left/right coordinates.

		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		int x = centreX - dx;
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, centreX - dx, centreY + dy - radius, centreX + dx, centreY + dy - radius);
		SDL_RenderDrawLine(renderer, centreX - dx, centreY - dy + radius, centreX + dx, centreY - dy + radius);
	}
}
