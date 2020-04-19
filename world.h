#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "random.h"

class World
{
    public:

    enum TileType
    {
        LAND,
        WATER
    };

    class Tile
    {
        public:

        Tile(TileType t, double f);

        void Step(unsigned int rate);

        double GrowthRate(unsigned int rate);
        double MaximumFoodValue(void);

        double ReduceByPercentage(double percentage);
        void IncreaseFood(double amount);

        unsigned char Red(void);
        unsigned char Green(void);
        unsigned char Blue(void);

        TileType Type(void) const { return type; }
        double Food(void) const { return food; }

        private:

        TileType type;
        double food;
    };

    World(double w, double h, double tileSize);

    void Step(unsigned int rate);

    inline World::Tile * GetTile(double x, double y)
    {
        int xIndex = (int) (x / tileSize);
        int yIndex = (int) (y / tileSize);

        xIndex = xIndex % tiles.size();
        yIndex = yIndex % tiles[xIndex].size();

        return tiles[xIndex][yIndex];
    }

    double Width(void) { return width; }
    double Height(void) { return height; }
    double TileSize(void) { return tileSize; }

    private:

    double width;
    double height;

    double tileSize;

    std::vector<std::vector<Tile *>> tiles;
};

#endif // WORLD_H
