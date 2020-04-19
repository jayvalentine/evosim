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

    inline double NeuronValue(int neuron)
    {
        // Base case. If we've got an input, return the input value.
        if (neuronTypes[neuron] == INPUT)
        {
            return inputValues[neuron];
        }

        // Find all neurons for which this one is the end, and calculate a weighted sum.
        double weightedSum = 0.0;

        for (int i = 0; i < synapses.size(); i++)
        {
            if (synapses[i]->End() == neuron)
            {
                weightedSum += (synapses[i]->Weight() * NeuronValue(synapses[i]->Start()));
            }
        }

        // The value of the neuron is the weighted sum passed through a sigmoid function.
        // Here we use a fast approximation.
        return weightedSum / (1 + fabs(weightedSum));
    }

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
