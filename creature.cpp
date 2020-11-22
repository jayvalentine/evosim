#include "creature.h"

// Constructor.
Creature::Creature(World * w, Point initialPosition, NeuralNetwork * n, Attributes attr, int gen, unsigned int rate)
{
    generation = gen;

    dead = false;

    world = w;

    x = initialPosition.X();
    y = initialPosition.Y();
    heading = Random::Double(0, 2);
    speed = 0.0;
    rotationalSpeed = 0.0;
    sizeFactor = 0.5;
    reproductionDelay = 30 * rate;
    age = 0;

    attributes = attr;

    netInputs = new double[n->Inputs().size()];
    netOutputs = new double[n->Outputs().size()];

    net = n;
}

// Destructor.
Creature::~Creature()
{
    delete net;
    delete[] netInputs;
    delete[] netOutputs;
}

Point Creature::GetPointInLine(double l, double h)
{
    double point_x = x + (l * cos(h));
    double point_y = y + (l * sin(h));

    if (point_x >= world->Width()) point_x -= world->Width();
    else if (point_x < 0) point_x = world->Width() + point_x;

    if (point_y >= world->Height()) point_y -= world->Height();
    else if (point_y < 0) point_y = world->Height() + point_y;

    return Point(point_x, point_y);
}

Creature::StepState Creature::Step(unsigned int rate)
{
    if (age > attributes.lifespan)
    {
        dead = true;
        return DEAD;
    }
    else age += 1.0 / rate;

    if (reproductionDelay > 0) reproductionDelay--;

    StepState state = NO_CHANGE;

    // Speed is a direct output of the network, so is already normalised by sigmoid function.
    netInputs[0] = ((speed * rate) * 2) - 1;

    // Rotational velocity is in rad/s. A direct output of the network, so already normalised by sigmoid function.
    netInputs[1] = (rotationalSpeed * rate);

    Point currentPosition = Point(x, y);

    // Tile colour. Each value is 0-255. Divide by 255/2 and subtract 1.
    double red = (world->GetTile(currentPosition)->Red() / 127.5) - 1;
    double green = (world->GetTile(currentPosition)->Green() / 127.5) - 1;
    double blue = (world->GetTile(currentPosition)->Blue() / 127.5) - 1;

    netInputs[2] = red;
    netInputs[3] = green;
    netInputs[4] = blue;

    // Size factor is between 0 and 1. Double and subtract one.
    netInputs[5] = (sizeFactor * 2) - 1;

    // Percentage of lifespan.
    netInputs[6] = ((age / attributes.lifespan) * 2) - 1;

    // Value of tile at sight-point.
    double sightDistance = (GetSize() / 2) + attributes.sightDistance;

    Point leftPoint = GetPointInLine(sightDistance, heading - 0.52);

    // Tile colour. Each value is 0-255. Divide by 255/2 and subtract 1.
    double seenRed_A = (world->GetTile(leftPoint)->Red() / 127.5) - 1;
    double seenGreen_A = (world->GetTile(leftPoint)->Green() / 127.5) - 1;
    double seenBlue_A = (world->GetTile(leftPoint)->Blue() / 127.5) - 1;

    netInputs[7] = seenRed_A;
    netInputs[8] = seenGreen_A;
    netInputs[9] = seenBlue_A;

    // Right sight-point.
    Point rightPoint = GetPointInLine(sightDistance, heading + 0.52);

    // Tile colour. Each value is 0-255. Divide by 255/2 and subtract 1.
    double seenRed_B = (world->GetTile(rightPoint)->Red() / 127.5) - 1;
    double seenGreen_B = (world->GetTile(rightPoint)->Green() / 127.5) - 1;
    double seenBlue_B = (world->GetTile(rightPoint)->Blue() / 127.5) - 1;

    netInputs[10] = seenRed_B;
    netInputs[11] = seenGreen_B;
    netInputs[12] = seenBlue_B;

    net->OutputValues(netInputs, netOutputs);

    double feedFactor = (netOutputs[2] + 1.0) / 10.0;

    // Feeding factor is 0.1. The creature consumes 10% of the food in the tile it's on.
    // Scale down because this is per-step, and the feeding rate is per-second.
    double food = world->GetTile(currentPosition)->ReduceByPercentage(feedFactor / rate);

    // Input energy is a proportion of food. Some is wasted.
    double inputEnergy = (food * 0.8);

    // Change speed and heading according to network.
    // These values need to be scaled down.
    speed = ((netOutputs[0] + 1) / 2) / rate;
    rotationalSpeed = netOutputs[1] / rate;

    //printf("Rotational speed: %.6f\n", rotationalSpeed);

    heading += rotationalSpeed;

    if (heading < 0.0) heading = (2.0 * M_PI) + heading;
    else if (heading > (2.0 * M_PI)) heading -= (2.0 * M_PI) ;

    double dx = (speed * attributes.maxSpeed) * cos(heading);
    double dy = (speed * attributes.maxSpeed) * sin(heading);

    x += dx;
    y += dy;

    if (x > world->Width()) x -= world->Width();
    else if (x < 0.0) x = world->Width() + x;

    if (y > world->Height()) y -= world->Height();
    else if (y < 0.0) y = world->Height() + y;

    // Desire to give birth is output 3.
    // If above or equal to 0, the creature may give birth.
    //
    // Note: allowing birth if the birth desire is 0 allows for those creatures for whom the birth neuron
    // is disconnected to still reproduce.
    if (netOutputs[3] >= 0 && reproductionDelay == 0) 
    {
        state = GIVE_BIRTH;

        // Creature cannot give birth again for a minute.
        reproductionDelay = 30 * rate;
    }

    // Calculate energy used.

    // Each unit of speed costs one unit of energy.
    // Each unit of rotational velocity costs one unit of energy.
    // Both are already scaled.
    double spentEnergy = (speed * attributes.maxSpeed) + rotationalSpeed + (sizeFactor * attributes.maxSize);

    // A creature in an environment in which it cannot breathe dies quickly.
    if ((attributes.breathing == WATER && world->GetTile(currentPosition)->Type() == World::TileType::LAND)
        || (attributes.breathing == LAND && world->GetTile(currentPosition)->Type() == World::TileType::WATER))
    {
        sizeFactor -= 0.05;
    }

    // Change size factor accordingly.
    double changeInSize = (inputEnergy - spentEnergy) / 500;
    sizeFactor += changeInSize / attributes.maxSize;

    // Giving birth requires 1/2 of size.
    if (state == GIVE_BIRTH) sizeFactor -= 0.5;

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
