#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <vector>
#include <tuple>

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

    int Start(void) { return start; }
    int End(void) { return end; }
    double Weight(void) { return weight; }

    void ChangeWeight(double newWeight);
    void ScaleWeight(double factor);

    private:

    int start;
    int end;
    double weight;
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

    std::vector<double> OutputValues(std::vector<double> inputs);

    void AddSynapse(int start, int end, double weight);
    void AddHiddenNeuron(int synapseIndex);

    double NeuronValue(int neuronIndex);

    std::vector<int> Inputs(void);
    std::vector<int> Outputs(void);
    std::vector<int> Hidden(void);
    std::vector<Synapse *> Synapses(void);

    NeuronType Type(int neuron);

    private:

    std::vector<Synapse *> synapses;
    std::vector<double> inputValues;
    std::vector<NeuronType> neuronTypes;
};

#endif // NEURALNETWORK_H
