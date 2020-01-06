#include "simulation.h"

Simulation::Simulation(World * w, int minCreatures)
{
    creatures = std::vector<std::shared_ptr<Creature>>();
    world = w;
    minimumCreatures = minCreatures;
}

void Simulation::AddInitialCreature(double initialX, double initialY)
{
    // Create a new neural network for the creature.
    NeuralNetwork * net = new NeuralNetwork(4, 4);

    // Mutate the network a bit.
    for (int i = 0; i < 5; i++)
    {
        Evolution::Mutate(net);
    }

    // Create a new shared_ptr for the creature;
    Creature * creature = new Creature(world, initialX, initialY, net);

    AddCreature(creature);
}

void Simulation::AddOffspringCreature(Creature * creature)
{
    // Create a new neural network for the creature and copy the synapses of the old one.
    NeuralNetwork * net = new NeuralNetwork(4, 4);

    for (auto s : creature->Net()->Synapses())
    {
        net->AddSynapse(s->Start(), s->End(), s->Weight());
    }

    // Mutate the network.
    Evolution::Mutate(net);

    // Create a new shared_ptr for the creature
    Creature * offspring = new Creature(world, creature->GetXPosition(), creature->GetYPosition(), net);

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
    world->Step();

    for (int i = 0; i < creatures.size(); i++)
    {
        Creature::StepState state = creatures[i]->Step();

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

    if (creaturesToAdd > 0) printf("Population below minimum. Injecting new life...\n");

    for (int i = 0; i < creaturesToAdd; i++)
    {
        AddInitialCreature(Random::Double(0, world->Width()), Random::Double(0, world->Height()));
    }
}
