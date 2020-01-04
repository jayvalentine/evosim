#include "creature.h"

// Constructor.
Creature::Creature(double initialX, double initialY, double initialSize, double worldWidth, double worldHeight)
{
    x = initialX;
    y = initialY;
    size = initialSize;

    maxX = worldWidth;
    maxY = worldHeight;

    heading = 0.0;
    speed = 0.0;
}

void Creature::Step(void)
{
    // Randomly change heading.
    heading += Random::Double(-0.2, 0.2);

    // Randomly change speed.
    speed += Random::Double(-0.2, 0.2);

    if (heading < 0.0) heading = 2.0 + heading;
    else if (heading > 2.0) heading -= 2.0;

    double dx = speed * cos(heading);
    double dy = speed * sin(heading);

    x += dx;
    y += dy;

    if (x > maxX) x -= maxX;
    else if (x < 0.0) x = maxX + x;

    if (y > maxY) y -= maxY;
    else if (y < 0.0) y = maxY + y;
}
