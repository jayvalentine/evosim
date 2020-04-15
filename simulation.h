#include <memory>
#include <cmath>

#include "world.h"
#include "creature.h"
#include "neuralnetwork.h"
#include "evolution.h"

class Simulation
{
    public:

    Simulation(World * w, int minCreatures, unsigned int rate);

    void Step(void);

    unsigned int SimulationTime(void);

    void AddInitialCreature(double initialX, double initialY);
    void AddOffspringCreature(Creature * creature);

    std::shared_ptr<Creature> GetCreature(int index) { return creatures[index]; };
    size_t CreatureCount(void) { return creatures.size(); };

    World * GetWorld(void) { return world; };

    private:

    void AddCreature(const Creature * creature);

    std::vector<std::shared_ptr<Creature>> creatures;
    World * world;

    // Minimum number of creatures to maintain in the world.
    int minimumCreatures;

    unsigned int stepRate;
    unsigned long steps;
};
