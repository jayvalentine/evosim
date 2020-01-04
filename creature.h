#ifndef CREATURE_H
#define CREATURE_H

class Creature
{
    public:

    // Constructor.
    Creature(double initialX, double initialY, double initialSize, double worldWidth, double worldHeight);

    // Perform one 'step' of this creature's lifespan.
    void Step(void);

    // Getters for private properties.
    double GetXPosition(void) { return x; }
    double GetYPosition(void) { return y; }

    double GetSize(void) { return size; }

    private:

    // X and Y position in the world of this creature.
    double x;
    double y;

    // Size of this creature.
    double size;

    // Maximum x and y positions.
    double maxX;
    double maxY;
};

#endif // CREATURE_H
