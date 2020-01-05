#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "random.h"

class World
{
    public:

    World(double w, double h, double tileSize);

    void Step(void);

    double GetTile(double x, double y);

    double Width(void) { return width; }
    double Height(void) { return height; }
    double TileSize(void) { return tileSize; }

    private:

    double width;
    double height;

    double tileSize;

    std::vector<std::vector<double>> tiles;
};

#endif // WORLD_H
