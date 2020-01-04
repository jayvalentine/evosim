#include "world.h"

World::World(double w, double h)
{
    width = w;
    height = h;
}

void World::Step(void)
{
    for (int i = 0; i < creatures.size(); i++)
    {
        creatures[i]->Step();
    }
}

void World::AddCreature(double initialX, double initialY, double initialSize)
{
    creatures.push_back(new Creature(initialX, initialY, initialSize, width, height));
}
