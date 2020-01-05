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
    rotationalSpeed = 0.0;

    attributes.red = Random::UInt(0, 255);
    attributes.green = Random::UInt(0, 255);
    attributes.blue = Random::UInt(0, 255);

    attributes.maxSpeed = Random::Double(1, 50);

    net = new NeuralNetwork(3, 2);
}

void Creature::Step(void)
{
    std::vector<double> inputs = std::vector<double>();

    // Rotational velocity is in rad/s. A direct output of the network, so already normalised by sigmoid function.
    inputs.push_back(rotationalSpeed * 60);

    // Speed is a direct output of the network, so is already normalised by sigmoid function.
    inputs.push_back(speed * 60);

    // Tile value is between 0 and 100. Divide by 50 and subtract 1.
    inputs.push_back((world->GetTile(x, y) / 50) - 1);

    std::vector<double> outputs = net->OutputValues(inputs);

    // Change speed and heading according to network.
    // One step is actually 1/60th of a second, so these values need to be scaled down.
    speed = outputs[0] / 60;
    heading += outputs[1] / 60;

    if (heading < 0.0) heading = 2.0 + heading;
    else if (heading > 2.0) heading -= 2.0;

    double dx = (speed * attributes.maxSpeed) * cos(heading);
    double dy = (speed * attributes.maxSpeed) * sin(heading);

    x += dx;
    y += dy;

    if (x > world->Width()) x -= world->Width();
    else if (x < 0.0) x = world->Width() + x;

    if (y > world->Height()) y -= world->Height();
    else if (y < 0.0) y = world->Height() + y;
}
