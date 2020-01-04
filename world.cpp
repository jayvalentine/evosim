#include "world.h"

World::World(double w, double h, double t)
{
    width = w;
    height = h;

    tileSize = t;

    creatures = std::vector<Creature *>();

    tiles = std::vector<std::vector<double>>();

    // Calculate number of tiles per row and column.
    int tilesPerRow = (int) (width / tileSize);
    int tilesPerColumn = (int) (height / tileSize);

    // Initialize tiles.
    for (int x = 0; x < tilesPerRow; x++)
    {
        tiles.push_back(std::vector<double>());

        for (int y = 0; y < tilesPerRow; y++)
        {
            tiles[x].push_back(Random::Double(0, 100));
        }
    }
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

double World::GetTile(double x, double y)
{
    int xIndex = (int) (x / tileSize);
    int yIndex = (int) (y / tileSize);

    return tiles[xIndex][yIndex];
}
