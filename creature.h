#ifndef CREATURE_H
#define CREATURE_H

#include <math.h>
#include <stdio.h>

#include "random.h"

class Creature
{
    public:

    // Constructor.
    Creature(double initialX, double initialY, double initialSize, double worldWidth, double worldHeight);

    // Perform one 'step' of this creature's lifespan.
    void Step(void);

    // Getters for private properties.
    double GetXPosition(void) { return x; }
    double GetYPosition(void) { return y; }

    double GetSize(void) { return size; }

    unsigned int Red(void) { return red; }
    unsigned int Green(void) { return green; }
    unsigned int Blue(void) { return blue; }

    private:

    // X and Y position in the world of this creature.
    double x;
    double y;

    // Size of this creature.
    double size;

    // Heading (in radians) of this creature.
    double heading;

    // Speed (in m/s) of this creature.
    double speed;

    // Maximum x and y positions.
    double maxX;
    double maxY;

    // Colour of this creature.
    unsigned int red;
    unsigned int green;
    unsigned int blue;
};

#endif // CREATURE_H
