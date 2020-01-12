#include "world.h"

#include <stdio.h>

World::Tile::Tile(World::TileType t, double f)
{
    type = t;
    food = f;
}

void World::Tile::Step(unsigned int rate)
{
    // Each tile grows by a factor of 1.005 per second.
    // This means that tiles which have been completely destroyed do not grow at all.
    food += food * (0.005 / rate);
    if (food > Tile::MaximumFoodValue) food = Tile::MaximumFoodValue;
}

double World::Tile::ReduceByPercentage(double percentage)
{
    double value = food * percentage;

    food -= value;

    return value;
}

unsigned char World::Tile::Red(void) const
{
    return 100;
}

unsigned char World::Tile::Green(void) const
{
    return 100 + (int)((food / MaximumFoodValue) * 155);
}

unsigned char World::Tile::Blue(void) const
{
    return 100;
}

World::World(double w, double h, double t)
{
    width = w;
    height = h;

    tileSize = t;

    tiles = std::vector<std::vector<Tile *>>();

    // Calculate number of tiles per row and column.
    int tilesPerRow = (int) (width / tileSize);
    int tilesPerColumn = (int) (height / tileSize);

    // Initialize tiles.
    for (int x = 0; x < tilesPerRow; x++)
    {
        tiles.push_back(std::vector<Tile *>());

        for (int y = 0; y < tilesPerRow; y++)
        {
            tiles[x].push_back(new Tile(GRASS, Random::Double(0, Tile::MaximumFoodValue)));
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

World::Tile * World::GetTile(double x, double y)
{
    int xIndex = (int) (x / tileSize);
    int yIndex = (int) (y / tileSize);

    xIndex = xIndex % tiles.size();
    yIndex = yIndex % tiles[xIndex].size();

    return tiles[xIndex][yIndex];
}
