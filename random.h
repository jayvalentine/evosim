#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <vector>

namespace Random
{
    void Init(unsigned int seed);

    double Double(double min, double max);
    unsigned int UInt(unsigned int min, unsigned int max);
    int Int(int min, int max);

    // Template function defined in header.
    template<class T> T Choice(std::vector<T> options);
};

template <class T> T Random::Choice(std::vector<T> options)
{
    unsigned int max = options.size() - 1;

    return options[UInt(0, max)];
}

#endif // RANDOM_H
