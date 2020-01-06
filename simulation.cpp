#include "simulation.h"

Simulation::Simulation(World * w, int minCreatures)
{
    creatures = std::vector<std::shared_ptr<Creature>>();
    world = w;
    minimumCreatures = minCreatures;
}

void Simulation::AddCreature(double initialX, double initialY)
{
    // Create a new shared_ptr for the creature;
    Creature * creature = new Creature(world, initialX, initialY);

    std::shared_ptr<Creature> ptr = std::make_shared<Creature>(*creature);

    creatures.push_back(ptr);
}

void Simulation::Step(void)
{
    world->Step();

    for (int i = 0; i < creatures.size(); i++)
    {
        creatures[i]->Step();
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
    int creaturesToAdd = minimumCreatures - creatures.size();

    if (creaturesToAdd > 0) printf("Population below minimum. Injecting new life...\n");
    
    for (int i = 0; i < creaturesToAdd; i++)
    {
        AddCreature(Random::Double(0, world->Width()), Random::Double(0, world->Height()));
    }
}
