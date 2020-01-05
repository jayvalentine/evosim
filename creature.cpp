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
    std::vector<double> inputs = std::vector<double>();

    // Heading is in radians, between 0 and 2. Normalisation is easy, just subtract one.
    inputs.push_back(heading - 1.0);

    // Speed is a direct output of the network, so is already normalised by sigmoid function.
    inputs.push_back(speed);

    // Tile value is between 0 and 100. Divide by 50 and subtract 1.
    inputs.push_back((world->GetTile(x, y) / 50) - 1);

    std::vector<double> outputs = net->OutputValues(inputs);

    // Change speed and heading according to network.
    speed = outputs[0];
    heading = outputs[1] + 1.0;

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
