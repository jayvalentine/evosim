#include "creature.h"

// Constructor.
Creature::Creature(double initialX, double initialY, double initialSize, double worldWidth, double worldHeight)
{
    x = initialX;
    y = initialY;
    size = initialSize;

    maxX = worldWidth;
    maxY = worldHeight;
}

void Creature::Step(void)
{
    x += 1.0;
    y += 1.0;

    if (x > maxX) x -= maxX;
    if (y > maxY) y -= maxY;
}
