#include "creature.h"

// Constructor.
Creature::Creature(double initialX, double initialY, double initialSize)
{
    x = initialX;
    y = initialY;
    size = initialSize;
}

void Creature::Step(void)
{
    x += 1.0;
    y += 1.0;
}
