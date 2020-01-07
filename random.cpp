#include "random.h"

void Random::Init(unsigned int seed)
{
    srand(seed);
}

double Random::Double(double min, double max)
{
    double r = (double) rand() / RAND_MAX;

    return min + r * (max - min);
}

unsigned int Random::UInt(unsigned int min, unsigned int max)
{
    unsigned int r = (unsigned int) rand() % (max - min + 1);

    return min + r;
}

int Random::Int(int min, int max)
{
    int r = (int) rand() % (max - min + 1);

    return min + r;
}
