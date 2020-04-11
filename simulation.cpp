#include "simulation.h"

Simulation::Simulation(World * w, int minCreatures, unsigned int rate)
{
    creatures = std::vector<std::shared_ptr<Creature>>();
    world = w;
    minimumCreatures = minCreatures;
    stepRate = rate;

    steps = 0;
}

void Simulation::AddInitialCreature(double initialX, double initialY)
{
    // Create a new neural network for the creature.
    NeuralNetwork * net = new NeuralNetwork(10, 4);

    // Initialise the network with 3 random synapses.
    for (int i = 0; i < 3; i++)
    {
        Evolution::AddRandomSynapse(net);
    }

    // Create the creature's attributes.
    Creature::Attributes attr;

    attr.red = Random::UInt(0, 255);
    attr.green = Random::UInt(0, 255);
    attr.blue = Random::UInt(0, 255);

    attr.maxSpeed = Random::Double(1, 50);

    attr.maxSize = Random::Double(25,  100);

    attr.lifespan = Random::Double(600, 1800);

    attr.sightDistance = Random::Double(100, 300);

    unsigned int roll = Random::UInt(0, 100);

    if (roll < 50) attr.breathing = Creature::BreathingType::LAND;
    else attr.breathing = Creature::BreathingType::WATER;

    // Create a new shared_ptr for the creature;
    Creature * creature = new Creature(world, initialX, initialY, net, attr, 0, stepRate);

    AddCreature(creature);
}

void Simulation::AddOffspringCreature(Creature * creature)
{
    // Create a new neural network for the creature and copy the synapses of the old one.
    NeuralNetwork * net = new NeuralNetwork(10, 4);

    for (auto s : creature->Net()->Synapses())
    {
        net->AddSynapse(s->Start(), s->End(), s->Weight());
    }

    // Mutate the network.
    Evolution::Mutate(net);

    // Clone the creature's attributes and mutate a bit.
    Creature::Attributes attr = creature->GetAttributes();

    // Color.
    attr.red += Random::Int(-10, 10);
    attr.green += Random::Int(-10, 10);
    attr.blue += Random::Int(-10, 10);

    // Cap these at 0-255.
    if (attr.red > 255) attr.red = 255;
    if (attr.green > 255) attr.green = 255;
    if (attr.blue > 255) attr.blue = 255;

    attr.maxSpeed += Random::Double(-5, 5);
    if (attr.maxSpeed < 1) attr.maxSpeed = 1;

    attr.maxSize += Random::Double(-10, 10);
    if (attr.maxSize < 25) attr.maxSize = 25;

    // Creature inherits lifespan directly.

    attr.sightDistance += Random::Double(-25, 25);
    if (attr.sightDistance < 0) attr.sightDistance = 0;
    else if (attr.sightDistance > 700) attr.sightDistance = 700;

    // Create a new shared_ptr for the creature
    Creature * offspring = new Creature(world, creature->GetXPosition(), creature->GetYPosition(), net, attr, creature->Generation() + 1, stepRate);

    AddCreature(offspring);
}

void Simulation::AddCreature(const Creature * creature)
{
    // Create a shared_ptr and add to the vector.
    std::shared_ptr<Creature> ptr = std::make_shared<Creature>(*creature);

    creatures.push_back(ptr);
}

void Simulation::Step(void)
{
    world->Step(stepRate);

    for (int i = 0; i < creatures.size(); i++)
    {
        Creature::StepState state = creatures[i]->Step(stepRate);

        if (state == Creature::StepState::GIVE_BIRTH)
        {
            AddOffspringCreature(creatures[i].get());
        }
    }

    // Remove any creatures which have died.
    std::vector<int> deadIndexes = std::vector<int>();
    for (int i = 0; i < creatures.size(); i++)
    {
        if (creatures[i]->Dead())
        {
            deadIndexes.push_back(i);
        }
    }

    // Starting from the back of the list we've just constructed,
    // remove the dead creatures.
    while (deadIndexes.size() > 0)
    {
        int index = deadIndexes[deadIndexes.size() - 1];
        deadIndexes.pop_back();

        creatures[index].reset();

        creatures.erase(creatures.begin() + index);
    }

    // If there are fewer creatures than the minimum, add creatures to pad out.
    int creaturesToAdd = (minimumCreatures - creatures.size()) + (minimumCreatures / 2);

    for (int i = 0; i < creaturesToAdd; i++)
    {
        AddInitialCreature(Random::Double(0, world->Width()), Random::Double(0, world->Height()));
    }

    steps++;
}

unsigned int Simulation::SimulationTime(void)
{
    // Divide the number of steps by the rate to get the time in seconds.
    return steps / stepRate;
}
