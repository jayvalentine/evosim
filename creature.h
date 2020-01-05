#ifndef CREATURE_H
#define CREATURE_H

#include <math.h>
#include <stdio.h>

#include "world.h"
#include "random.h"
#include "neuralnetwork.h"

class Creature
{
    public:

    struct Attributes
    {
        // Colour in RGB format.
        unsigned char red;
        unsigned char green;
        unsigned char blue;

        // Maximum speed in m/s of the creature.
        double maxSpeed;
    };

    // Constructor.
    Creature(World * w, double initialX, double initialY, double initialSize);

    // Perform one 'step' of this creature's lifespan.
    void Step(void);

    // Getters for private properties.
    double GetXPosition(void) { return x; }
    double GetYPosition(void) { return y; }

    double GetSize(void) { return size; }

    unsigned int Red(void) { return attributes.red; }
    unsigned int Green(void) { return attributes.green; }
    unsigned int Blue(void) { return attributes.blue; }

    NeuralNetwork * Net(void) { return net; }

    Attributes GetAttributes(void) { return attributes; }

    private:

    Attributes attributes;

    // X and Y position in the world of this creature.
    double x;
    double y;

    // Size of this creature.
    double size;

    // Heading (in radians) of this creature.
    double heading;

    // Rotational speed (in rad/s) of this creature.
    double rotationalSpeed;

    // Speed (in m/s) of this creature.
    double speed;

    // The world in which this creature exists.
    World * world;

    // This creature's neural network.
    NeuralNetwork * net;
};

#endif // CREATURE_H
