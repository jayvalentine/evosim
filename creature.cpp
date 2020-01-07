#include "creature.h"

// Constructor.
Creature::Creature(World * w, double initialX, double initialY, NeuralNetwork * n, int gen)
{
    generation = gen;

    dead = false;

    world = w;

    x = initialX;
    y = initialY;
    heading = 0.0;
    speed = 0.0;
    rotationalSpeed = 0.0;
    sizeFactor = 0.5;

    attributes.red = Random::UInt(0, 255);
    attributes.green = Random::UInt(0, 255);
    attributes.blue = Random::UInt(0, 255);

    attributes.maxSpeed = Random::Double(1, 50);

    attributes.maxSize = Random::Double(1, 100);

    net = n;
}

// Destructor.
Creature::~Creature()
{
    delete net;
}

Creature::StepState Creature::Step(unsigned int rate)
{
    StepState state = NO_CHANGE;

    std::vector<double> inputs = std::vector<double>();

    // Rotational velocity is in rad/s. A direct output of the network, so already normalised by sigmoid function.
    inputs.push_back(rotationalSpeed * rate);

    // Speed is a direct output of the network, so is already normalised by sigmoid function.
    inputs.push_back(speed * rate);

    // Tile value is between 0 and max. Divide by max / 2 and subtract 1.
    inputs.push_back((world->GetTile(x, y) / (world->MaximumFoodValue() / 2)) - 1);

    // Size factor is between 0 and 1. Double and subtract one.
    inputs.push_back((sizeFactor * 2) - 1);

    std::vector<double> outputs = net->OutputValues(inputs);

    double feedFactor = (outputs[2] + 1.0) / 4.0;

    // Feeding factor is 0.1. The creature consumes 10% of the food in the tile it's on.
    // Scale down because this is per-step, and the feeding rate is per-second.
    double food = world->ReduceTileByPercentage(x, y, feedFactor / rate);

    // Input energy is a proportion of food. Some is wasted.
    double inputEnergy = (food * 0.8);

    // Change speed and heading according to network.
    // These values need to be scaled down.
    speed = outputs[0] / rate;
    rotationalSpeed = outputs[1] / rate;

    heading += rotationalSpeed;

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

    // Desire to give birth is output 3.
    // If above 0.2, the creature may give birth.
    if (outputs[3] > 0.2) state = GIVE_BIRTH;

    // Calculate energy used.

    // Each unit of speed costs one unit of energy.
    // Each unit of rotational velocity costs one unit of energy.
    // Both are already scaled.
    double spentEnergy = (speed * attributes.maxSpeed) + rotationalSpeed;

    // Change size factor accordingly.
    double changeInSize = (inputEnergy - spentEnergy) / 10;
    sizeFactor = changeInSize / attributes.maxSize;

    // Giving birth requires 1/10th of size.
    if (state == GIVE_BIRTH) sizeFactor -= 0.1;

    // If size factor < 0.1, the creature is dead.
    if (sizeFactor < 0.1)
    {
        dead = true;
        state = DEAD;
    }

    // Cap size factor at 1.
    if (sizeFactor > 1.0) sizeFactor = 1.0;

    // Return the state, this is needed by the simulator.
    return state;
}
