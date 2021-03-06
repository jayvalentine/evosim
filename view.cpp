#include "view.h"

View::View(SDL_Window * window, SDL_Window * netWindow, Simulation * sim, double initialX, double initialY, double initialScale)
    : cameraPosition(initialX, initialY)
{
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    netWindowReference = netWindow;

    netRenderer = SDL_CreateRenderer(netWindow, -1, SDL_RENDERER_ACCELERATED);

    simReference = sim;
    cameraScale = initialScale;

    focusCreature = NULL;
    focusIndex = -1;

    // Globally used font.
    font = TTF_OpenFont("VeraMono.ttf", 20);
}

View::~View()
{
    SDL_DestroyRenderer(renderer);
}

void View::RenderNet(NeuralNetwork * net)
{
    std::vector<int> inputs = net->Inputs();
    std::vector<int> outputs = net->Outputs();
    std::vector<int> hidden = net->Hidden();

    // First draw the synapses. That way the neurons will be drawn over them.
    std::vector<Synapse *> synapses = net->Synapses();

    for (int i = 0; i < synapses.size(); i++)
    {
        // From an input to an output.
        Synapse * s = synapses[i];

        NeuralNetwork::NeuronType startType = net->Type(s->Start());
        NeuralNetwork::NeuronType endType = net->Type(s->End());

        int startX;
        int endX;

        int startY;
        int endY;
        
        if (startType == NeuralNetwork::NeuronType::INPUT)
        {
            startX = 25;

            startY = 25 + (35 * s->Start());

            if (endType == NeuralNetwork::NeuronType::OUTPUT)
            {
                endX = 375;

                endY = 25 + (35 * (s->End() - inputs.size()));
            }
            else
            {
                endX = 200;

                endY = 25 + (35 * (s->End() - inputs.size() - outputs.size()));
            }
        }
        else
        {
            startX = 200;
            startY = 25 + (35 * (s->Start() - inputs.size() - outputs.size()));

            endX = 375;
            endY = 25 + (35 * (s->End() - inputs.size()));
        }
        
        SDL_SetRenderDrawColor(netRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(netRenderer, startX, startY, endX, endY);
    }

    for (int i = 0; i < inputs.size(); i++)
    {
        int x = 25;
        int y = 25 + (35 * inputs[i]);

        DrawCircle(netRenderer, x, y, 15, 255, 255, 255);

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
        FillCircle(netRenderer, x, y, 14, red, green, blue);
    }

    for (int i = 0; i < outputs.size(); i++)
    {
        int x = 375;
        int y = 25 + (35 * (outputs[i] - inputs.size()));

        DrawCircle(netRenderer, x, y, 15, 255, 255, 255);

        // Determine the colour of the neuron.
        // Red if the neuron is negative,
        // White if the neuron is 0,
        // Green if the neuron is positive.

        unsigned int red = 255;
        unsigned int green = 255;
        unsigned int blue = 255;

        double value = net->NeuronValue(outputs[i]);

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
        FillCircle(netRenderer, x, y, 14, red, green, blue);
    }

    // Now draw any hidden neurons.
    for (int i = 0; i < hidden.size(); i++)
    {
        int x = 200;
        int y = 25 + (35 * (hidden[i] - inputs.size() - outputs.size()));

        DrawCircle(netRenderer, x, y, 15, 255, 255, 255);

        // Determine the colour of the neuron.
        // Red if the neuron is negative,
        // White if the neuron is 0,
        // Green if the neuron is positive.

        unsigned int red = 255;
        unsigned int green = 255;
        unsigned int blue = 255;

        double value = net->NeuronValue(hidden[i]);

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
        FillCircle(netRenderer, x, y, 14, red, green, blue);
    }
}

void View::RenderInfo(Creature * creature)
{
    const int startPosition = 700;

    const int leftColumn = 10;
    const int rightColumn = 310;

    char buf[100];

    RenderText(netRenderer, "Attributes:", leftColumn, startPosition + 10);

    sprintf(buf, "Generation: %d", creature->Generation());

    RenderText(netRenderer, buf, leftColumn, startPosition + 40);

    sprintf(buf, "Age:        %d/%d", (int)creature->GetAge(), (int)creature->GetAttributes().lifespan);

    RenderText(netRenderer, buf, leftColumn, startPosition + 70);

    sprintf(buf, "Speed:      %3.1fm/s", creature->GetSpeed());

    RenderText(netRenderer, buf, leftColumn, startPosition + 100);

    sprintf(buf, "Size:       %3.1fm", creature->GetSize());

    RenderText(netRenderer, buf, leftColumn, startPosition + 130);

    sprintf(buf, "Heading:    %1.1frad", creature->GetHeading());

    RenderText(netRenderer, buf, leftColumn, startPosition + 160);

    char breathing[100];

    if (creature->GetAttributes().breathing == Creature::BreathingType::LAND)
    {
        sprintf(breathing, "Terrestrial");
    }
    else if (creature->GetAttributes().breathing == Creature::BreathingType::WATER)
    {
        sprintf(breathing, "Aquatic");
    }
    else
    {
        sprintf(breathing, "Amphibious");
    }

    sprintf(buf, "Affinity:   %s", breathing);

    RenderText(netRenderer, buf, rightColumn, startPosition + 10);
    
}

void View::RenderText(SDL_Renderer * r, const char * text, int x, int y)
{
    SDL_Color textColor = {255, 255, 255};

    SDL_Surface * textSurface = TTF_RenderText_Solid(font, text, textColor);

    SDL_Texture * textTexture = SDL_CreateTextureFromSurface(r, textSurface);

    // A rectangle of where we want this text to be.
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = textSurface->w;
    dst.h = textSurface->h;

    SDL_RenderCopy(r, textTexture, NULL, &dst);

    // Free the surface and texture.
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void View::RenderSimulationInfo(const char * buf)
{
    int width;
    int height;

    SDL_GetRendererOutputSize(renderer, &width, &height);

    int startPosition = height - 50;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Draw a black rectangle in the info box.
    SDL_Rect rect;
    rect.x = 0;
    rect.y = startPosition;
    rect.h = 50;
    rect.w = width;

    SDL_RenderFillRect(renderer, &rect);

    RenderText(renderer, buf, 10, startPosition + 10);

    SDL_RenderPresent(renderer);
}

void View::Render(void)
{
    // If we're focusing on a creature, set the camera position.
    if (focusCreature)
    {
        if (focusCreature->Dead())
        {
            focusCreature.reset();
            SDL_HideWindow(netWindowReference);
        }
        else
        {
            cameraPosition = focusCreature->GetPosition();

            // Show the information window.
            SDL_ShowWindow(netWindowReference);

            SDL_SetRenderDrawColor(netRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(netRenderer);

            // Render the creature's neural net to the information window.
            RenderNet(focusCreature->Net());

            // Render the creature's info to the window.
            RenderInfo(focusCreature.get());

            SDL_RenderPresent(netRenderer);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    int width;
    int height;

    SDL_GetRendererOutputSize(renderer, &width, &height);

    // Leave 50 pixels at the bottom of the screen.
    // This is for the simulation info-bar.
    height -= 50;

    // Determine the width and height (in metres) of the camera.
    double cameraWidth = width / cameraScale;
    double cameraHeight = height / cameraScale;

    // Now we can determine the boundaries of the camera in metres, and so
    // work out what we can display.
    double cameraLeft = cameraPosition.X() - (cameraWidth / 2);
    if (cameraLeft < 0.0) cameraLeft = 0.0;
    else if (cameraLeft > (simReference->GetWorld()->Width() - cameraWidth)) cameraLeft = simReference->GetWorld()->Width() - cameraWidth;

    double cameraRight = cameraLeft + cameraWidth;

    double cameraTop = cameraPosition.Y() - (cameraHeight / 2);
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

            World::Tile * tile = simReference->GetWorld()->GetTile(Point(realX, realY));

            if (tileBottomPixels < viewY) 
            {
                viewY++;
                continue;
            }

            // Create a rectangle for this tile.
            SDL_Rect displayTile;

            displayTile.x = viewX;
            displayTile.y = viewY;
            displayTile.w = (tileRightPixels - viewX);
            displayTile.h = (tileBottomPixels - viewY);

            SDL_SetRenderDrawColor(renderer, tile->Red(), tile->Green(), tile->Blue(), SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &displayTile);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &displayTile);

            // Now move onto the next tile.
            
            viewY = tileBottomPixels + 1;
        }

        // Now move onto the next column.
        viewX = tileRightPixels + 1;
    }

    // Iterate over the creatures and draw each one.
    for (int i = 0; i < simReference->CreatureCount(); i++)
    {
        std::shared_ptr<Creature> creature = simReference->GetCreature(i);

        if (!creature) continue;

        Point creaturePosition = creature->GetPosition();

        if (creaturePosition.X() < cameraLeft) continue;
        if (creaturePosition.X() > cameraRight) continue;
        
        if (creaturePosition.Y() < cameraTop) continue;
        if (creaturePosition.Y() > cameraBottom) continue;

        DrawCreature(renderer, creature.get(), cameraLeft, cameraTop);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Draw a black rectangle in the info box.
    SDL_Rect rect;
    rect.x = 0;
    rect.y = height;
    rect.h = 50;
    rect.w = width;

    SDL_RenderFillRect(renderer, &rect);

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
    focusCreature.reset();

    double newCameraX = cameraPosition.X() - PanSpeed();
    if (CameraLeft() < 0.0) newCameraX += PanSpeed();
    cameraPosition = Point(newCameraX, cameraPosition.Y());
}

void View::PanRight(void)
{
    focusCreature.reset();

    double newCameraX = cameraPosition.X() + PanSpeed();
    if (CameraRight() > simReference->GetWorld()->Width()) newCameraX -= PanSpeed();
    cameraPosition = Point(newCameraX, cameraPosition.Y());
}

void View::PanUp(void)
{
    focusCreature.reset();

    double newCameraY = cameraPosition.Y() - PanSpeed();
    if (CameraTop() < 0.0) newCameraY += PanSpeed();
    cameraPosition = Point(cameraPosition.X(), newCameraY);
}

void View::PanDown(void)
{
    focusCreature.reset();
    
    double newCameraY = cameraPosition.Y() + PanSpeed();
    if (CameraBottom() > simReference->GetWorld()->Height()) newCameraY -= PanSpeed();
    cameraPosition = Point(cameraPosition.X(), newCameraY);
}

double View::XCoordinate(void)
{
    return cameraPosition.X();
}

double View::YCoordinate(void)
{
    return cameraPosition.Y();
}

double View::CameraLeft(void)
{
    int width;
    int height;

    SDL_GetRendererOutputSize(renderer, &width, &height);

    double cameraWidth = width / cameraScale;
    double cameraLeft = cameraPosition.X() - (cameraWidth / 2);

    return cameraLeft;
}

double View::CameraRight(void)
{
    int width;
    int height;

    SDL_GetRendererOutputSize(renderer, &width, &height);

    double cameraWidth = width / cameraScale;
    double cameraRight = cameraPosition.X() + (cameraWidth / 2);

    return cameraRight;
}

double View::CameraTop(void)
{
    int width;
    int height;

    SDL_GetRendererOutputSize(renderer, &width, &height);

    double cameraHeight = height / cameraScale;
    double cameraTop = cameraPosition.Y() - (cameraHeight / 2);

    return cameraTop;
}

double View::CameraBottom(void)
{
    int width;
    int height;

    SDL_GetRendererOutputSize(renderer, &width, &height);

    double cameraHeight = height / cameraScale;
    double cameraBottom = cameraPosition.Y() + (cameraHeight / 2);

    return cameraBottom;
}

void View::SelectFirst(void)
{
    // Return if there are no creatures.
    if (simReference->CreatureCount() == 0) return;

    focusCreature = simReference->GetCreature(0);
    focusIndex = 0;
}

void View::SelectLast(void)
{
    // Return if there are no creatures.
    if (simReference->CreatureCount() == 0) return;

    focusCreature = simReference->GetCreature(simReference->CreatureCount() - 1);
    focusIndex = simReference->CreatureCount() - 1;
}

void View::SelectHighestGeneration(void)
{
    std::shared_ptr<Creature> selected = simReference->GetCreature(0);
    focusIndex = 0;

    for (int i = 1; i < simReference->CreatureCount(); i++)
    {
        if (selected && simReference->GetCreature(i)->Generation() > selected->Generation())
        {
            selected = simReference->GetCreature(i);
            focusIndex = i;
        }
    }

    focusCreature = selected;
}

void View::SelectPrevious(void)
{
    focusIndex--;
    if (focusIndex < 0) focusIndex = simReference->CreatureCount() - 1;

    focusCreature = simReference->GetCreature(focusIndex);
}

void View::SelectNext(void)
{
    focusIndex++;
    if (focusIndex >= simReference->CreatureCount()) focusIndex = 0;

    focusCreature = simReference->GetCreature(focusIndex);
}

void View::HandleClick(int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Now we can determine the boundaries of the camera in metres, and so
    // work out what we can display.

    // First, calculate the real-world position of the click.
    double realX = CameraLeft() + (x / cameraScale);
    double realY = CameraTop() + (y / cameraScale);

    // Is this position a creature?
    for (int i = 0; i < simReference->CreatureCount(); i++)
    {
        std::shared_ptr<Creature> creature = simReference->GetCreature(i);

        if (!creature) continue;

        // Calculate distance from this creature to the clicked point.
        double xDist = fabs(realX - creature->GetPosition().X());
        double yDist = fabs(realY - creature->GetPosition().Y());

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
    Point creaturePosition = creature->GetPosition();

    double creatureSize = creature->GetSize();

    // If we get here then the creature is within the bounds of the camera,
    // so lets calculate its pixel position.

    double creatureWithinX = creaturePosition.X() - cameraLeft;
    double creatureWithinY = creaturePosition.Y() - cameraTop;

    // Scale both to get the pixel position.
    int creaturePixelX = (int) (creatureWithinX * cameraScale);
    int creaturePixelY = (int) (creatureWithinY * cameraScale);
    int creaturePixelRadius = (int) ((creatureSize / 2) * cameraScale);

    double sightDistance = (creature->GetSize() / 2) + creature->GetAttributes().sightDistance;
    int sightPixelDistance = sightDistance * cameraScale;

    double eyeRotation = creature->GetAttributes().eyeRotation;

    int endX_A = creaturePixelX + (sightPixelDistance * cos(creature->GetHeading() - eyeRotation));
    int endY_A = creaturePixelY + (sightPixelDistance * sin(creature->GetHeading() - eyeRotation));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    SDL_RenderDrawLine(renderer, creaturePixelX, creaturePixelY, endX_A, endY_A);

    int endX_B = creaturePixelX + (sightPixelDistance * cos(creature->GetHeading() + eyeRotation));
    int endY_B = creaturePixelY + (sightPixelDistance * sin(creature->GetHeading() + eyeRotation));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    SDL_RenderDrawLine(renderer, creaturePixelX, creaturePixelY, endX_B, endY_B);

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

    // These elements look terrible at small scales, so only draw them if scale > 0.3.
    // This will also improve performance at those small scales.
    if (cameraScale <= 0.3) return;

    // Draw a black line from the creature's centre to the edge.
    int dx = creaturePixelRadius * cos(creature->GetHeading());
    int dy = creaturePixelRadius * sin(creature->GetHeading());

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderDrawLine(renderer, creaturePixelX, creaturePixelY, creaturePixelX + dx, creaturePixelY + dy);
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
