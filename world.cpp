#include "world.h"

#include <stdio.h>

Point::Point(double x, double y)
{
    _x = x;
    _y = y;
}

World::Tile::Tile(World::TileType t, double f)
{
    type = t;
    food = f;
}

void World::Tile::Step(unsigned int rate)
{
    if (food < 0) food = 0;

    if (food == 0)
    {
        double roll = Random::Double(0, 1);
        if (roll < 0.01) food = 10;
    }

    // Each tile grows by a factor of 1.005 per second.
    // This means that tiles which have been completely destroyed do not grow at all.
    food += food * GrowthRate(rate);
    if (food > MaximumFoodValue()) food = MaximumFoodValue();
}

double World::Tile::GrowthRate(unsigned int rate)
{
    if (type == TileType::LAND) return (0.03 / rate);
    else return (0.003 / rate);
}

double World::Tile::ReduceByPercentage(double percentage)
{
    double value = food * percentage;

    food -= value;

    return value;
}

void World::Tile::IncreaseFood(double amount)
{
    food += amount;
    if (food > MaximumFoodValue()) food = MaximumFoodValue();
}

World::World(double w, double h, double t)
{
    width = w;
    height = h;

    tileSize = t;

    std::vector<std::vector<TileType>> tileTypes = std::vector<std::vector<TileType>>();

    // Calculate number of tiles per row and column.
    int tilesPerRow = (int) (width / tileSize);
    int tilesPerColumn = (int) (height / tileSize);

    // Determine the type of each tile. Initially, all tiles are water.
    for (int x = 0; x < tilesPerRow; x++)
    {
        tileTypes.push_back(std::vector<TileType>());

        for (int y = 0; y < tilesPerColumn; y++)
        {
            tileTypes[x].push_back(WATER);
        }
    }

    // Create landmasses using a "drunken man" method.
    // Start in a random point, and for 200 'turns', 'walk' in a random direction.
    for (int i = 0; i < 10; i++)
    {
        int x = Random::Int(0, tileTypes.size());
        int y = Random::Int(0, tileTypes[x].size());

        for (int t = 0; t < 500; t++)
        {
            tileTypes[x][y] = LAND;

            x += Random::Int(-1, 1);
            y += Random::Int(-1, 1);

            if (x < 0) x = tileTypes.size() - 1;
            else if (x >= tileTypes.size()) x = 0;

            if (y < 0) y = tileTypes[0].size() - 1;
            else if (y >= tileTypes[0].size()) y = 0;
        }
    }

    // Now initialize the actual tile objects.
    tiles = std::vector<std::vector<Tile *>>();

    for (int x = 0; x < tileTypes.size(); x++)
    {
        tiles.push_back(std::vector<Tile *>());

        for (int y = 0; y < tileTypes[x].size(); y++)
        {
            tiles[x].push_back(new Tile(tileTypes[x][y], Random::Double(0, 100000)));
        }
    }
}

void World::Step(unsigned int rate)
{
    
    for (int x = 0; x < tiles.size(); x++)
    {
        for (int y = 0; y < tiles[x].size(); y++)
        {
            tiles[x][y]->Step(rate);
        }
    }
}
