#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <vector>
#include <tuple>
#include <utility>
#include <map>

#include <math.h>

#include "random.h"

class Synapse
{
    public:

    Synapse(int s, int e, double w)
    {
        start = s;
        end = e;
        weight = w;
    };

    inline int Start(void) { return start; }
    inline int End(void) { return end; }
    inline double Weight(void) { return weight; }

    void ChangeWeight(double newWeight);
    void ScaleWeight(double factor);

    private:

    int start;
    int end;
    double weight;
    bool cached;
    bool cachedValue;
};

class NeuralNetwork
{
    public:

    enum NeuronType
    {
        INPUT,
        OUTPUT,
        HIDDEN
    };

    NeuralNetwork(int inputs, int outputs, int hidden);

    ~NeuralNetwork();

    void OutputValues(double * inputs, double * outputs);

    void AddSynapse(int start, int end, double weight);
    void AddHiddenNeuron(int synapseIndex);

    double NeuronValue(int neuron);

    std::vector<int> Inputs(void);
    std::vector<int> Outputs(void);
    std::vector<int> Hidden(void);
    std::vector<Synapse *> Synapses(void);

    NeuronType Type(int neuron);

    private:

    std::vector<Synapse *> synapses;
    std::vector<double> inputValues;
    std::vector<NeuronType> neuronTypes;
    unsigned int outputSize;
    unsigned int inputSize;
};

#endif // NEURALNETWORK_H
