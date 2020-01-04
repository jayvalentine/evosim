#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "creature.h"

class World
{
    public:

    World(double w, double h);

    void Step(void);

    void AddCreature(double initialX, double initialY, double initialSize);

    Creature * GetCreature(int index) { return creatures[index]; };
    size_t CreatureCount(void) { return creatures.size(); };

    double Width(void) { return width; }
    double Height(void) { return height; }

    private:

    double width;
    double height;

    std::vector<Creature *> creatures;
};

#endif // WORLD_H
