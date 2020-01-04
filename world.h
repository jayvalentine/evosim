#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "creature.h"
#include "random.h"

class World
{
    public:

    World(double w, double h, double tileSize);

    void Step(void);

    void AddCreature(double initialX, double initialY, double initialSize);

    Creature * GetCreature(int index) { return creatures[index]; };
    size_t CreatureCount(void) { return creatures.size(); };

    double GetTile(double x, double y);

    double Width(void) { return width; }
    double Height(void) { return height; }
    double TileSize(void) { return tileSize; }

    private:

    double width;
    double height;

    double tileSize;

    std::vector<Creature *> creatures;

    std::vector<std::vector<double>> tiles;
};

#endif // WORLD_H
