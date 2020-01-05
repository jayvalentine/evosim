#include "world.h"
#include "creature.h"

class Simulation
{
    public:

    Simulation(World * w);

    void Step(void);

    void AddCreature(double initialX, double initialY, double initialSize);

    Creature * GetCreature(int index) { return creatures[index]; };
    size_t CreatureCount(void) { return creatures.size(); };

    World * GetWorld(void) { return world; };

    private:

    std::vector<Creature *> creatures;
    World * world;
};
