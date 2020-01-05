#include "creature.h"

// Constructor.
Creature::Creature(World * w, double initialX, double initialY, double initialSize)
{
    world = w;

    x = initialX;
    y = initialY;
    size = initialSize;
    heading = 0.0;
    speed = 0.0;

    red = Random::UInt(0, 255);
    green = Random::UInt(0, 255);
    blue = Random::UInt(0, 255);

    net = new NeuralNetwork(3, 2);
}

void Creature::Step(void)
{
    std::vector<double> inputs = {heading, speed, world->GetTile(x, y)};

    std::vector<double> outputs = net->Outputs(inputs);

    // Change speed and heading according to network.
    speed = outputs[0];
    heading = outputs[1];

    if (heading < 0.0) heading = 2.0 + heading;
    else if (heading > 2.0) heading -= 2.0;

    double dx = speed * cos(heading);
    double dy = speed * sin(heading);

    x += dx;
    y += dy;

    if (x > world->Width()) x -= world->Width();
    else if (x < 0.0) x = world->Width() + x;

    if (y > world->Height()) y -= world->Height();
    else if (y < 0.0) y = world->Height() + y;
}
