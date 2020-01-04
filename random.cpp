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
