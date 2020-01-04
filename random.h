#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

namespace Random
{
    void Init(unsigned int seed);

    double Double(double min, double max);
    unsigned int UInt(unsigned int min, unsigned int max);
};

#endif // RANDOM_H
