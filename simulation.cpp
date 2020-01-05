#include "simulation.h"

Simulation::Simulation(World * w)
{
    creatures = std::vector<Creature *>();
    world = w;
}

void Simulation::AddCreature(double initialX, double initialY, double initialSize)
{
    creatures.push_back(new Creature(world, initialX, initialY, initialSize));
}

void Simulation::Step(void)
{
    world->Step();

    for (int i = 0; i < creatures.size(); i++)
    {
        creatures[i]->Step();
    }
}
