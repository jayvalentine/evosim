#include "world.h"

World::World(double w, double h, double t)
{
    width = w;
    height = h;

    tileSize = t;

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
            tiles[x].push_back(Random::Double(0, maxFood));
        }
    }
}

void World::Step(unsigned int rate)
{
}

double World::GetTile(double x, double y)
{
    int xIndex = (int) (x / tileSize);
    int yIndex = (int) (y / tileSize);

    xIndex = xIndex % tiles.size();
    yIndex = yIndex % tiles[xIndex].size();

    return tiles[xIndex][yIndex];
}

void World::SetTile(double x, double y, double value)
{
    int xIndex = (int) (x / tileSize);
    int yIndex = (int) (y / tileSize);

    xIndex = xIndex % tiles.size();
    yIndex = yIndex % tiles[xIndex].size();

    tiles[xIndex][yIndex] = value;
}

double World::ReduceTileByPercentage(double x, double y, double percentage)
{
    double value = GetTile(x, y) * percentage;

    SetTile(x, y, GetTile(x, y) - value);

    return value;
}
