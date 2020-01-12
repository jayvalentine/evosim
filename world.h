#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "random.h"

class World
{
    public:

    enum TileType
    {
        GRASS,
        WATER
    };

    class Tile
    {
        public:

        static constexpr double MaximumFoodValue = 100000.0;

        Tile(TileType t, double f);

        void Step(unsigned int rate);

        double ReduceByPercentage(double percentage);

        unsigned char Red(void) const;
        unsigned char Green(void) const;
        unsigned char Blue(void) const;

        double Food(void) const { return food; }

        private:

        TileType type;
        double food;
    };

    World(double w, double h, double tileSize);

    void Step(unsigned int rate);

    World::Tile * GetTile(double x, double y);

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
