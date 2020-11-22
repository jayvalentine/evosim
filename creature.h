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

    // Possible states of a creature following a step.
    enum StepState
    {
        NO_CHANGE,
        GIVE_BIRTH,
        DEAD
    };

    enum BreathingType
    {
        LAND,
        WATER,
        BOTH
    };

    struct Attributes
    {
        // Colour in RGB format.
        unsigned char red;
        unsigned char green;
        unsigned char blue;

        // Maximum speed in m/s of the creature.
        double maxSpeed;

        // Maximum size (diameter) in m of the creature.
        double maxSize;

        // Lifespan (in seconds) of the creature.
        double lifespan;

        // Sight distance in m of the creature.
        double sightDistance;

        // Breathing capability of this creature.
        BreathingType breathing;

        // How far (in radians) the creature's eyes are from its front.
        double eyeRotation;
    };

    // Constructor.
    Creature(World * w, Point initialPosition, NeuralNetwork * n, Attributes attr, int gen, unsigned int rate);

    // Destructor.
    ~Creature();

    bool Dead(void) { return dead; }

    // Perform one 'step' of this creature's lifespan.
    StepState Step(unsigned int rate);

    // Getters for private properties.
    int Generation(void) { return generation; }
    Point GetPosition(void) { return Point(x, y); }
    double GetHeading(void) { return heading; }

    double GetSize(void) { return sizeFactor * attributes.maxSize; }
    double GetSpeed(void) { return speed * attributes.maxSpeed; }

    double GetAge(void) { return age; }

    unsigned int Red(void) { return attributes.red; }
    unsigned int Green(void) { return attributes.green; }
    unsigned int Blue(void) { return attributes.blue; }

    NeuralNetwork * Net(void) { return net; }

    Attributes GetAttributes(void) { return attributes; }

    private:

    Point GetPointInLine(double l, double h);

    int generation;

    Attributes attributes;

    // True if the creature has died, false otherwise.
    bool dead;

    // X and Y position in the world of this creature.
    double x;
    double y;

    // Size of this creature, as a percentage of its maximum.
    double sizeFactor;

    // Heading (in radians) of this creature.
    double heading;

    // Rotational speed (in rad/s) of this creature.
    double rotationalSpeed;

    // Speed (in m/s) of this creature.
    double speed;

    // Number of steps until the creature is ready to reproduce.
    unsigned int reproductionDelay;

    // Age of this creature.
    double age;

    // The world in which this creature exists.
    World * world;

    // This creature's neural network.
    NeuralNetwork * net;
    double * netInputs;
    double * netOutputs;
};

#endif // CREATURE_H
