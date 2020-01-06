#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "random.h"

class World
{
    public:

    World(double w, double h, double tileSize);

    void Step(void);

    double MaximumFoodValue(void) { return maxFood; }

    double GetTile(double x, double y);
    void SetTile(double x, double y, double value);

    double ReduceTileByPercentage(double x, double y, double percentage);

    double Width(void) { return width; }
    double Height(void) { return height; }
    double TileSize(void) { return tileSize; }

    private:

    const double maxFood = 100000.0;

    double width;
    double height;

    double tileSize;

    std::vector<std::vector<double>> tiles;
};

#endif // WORLD_H
