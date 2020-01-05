#include "neuralnetwork.h"

NeuralNetwork::NeuralNetwork(int inputs, int outputs)
{
    neuronTypes = std::vector<NeuronType>();
    inputValues = std::vector<double>();

    neuronTypes.resize(inputs + outputs);
    inputValues.resize(inputs);

    for (int i = 0; i < inputs; i++)
    {
        neuronTypes[i] = INPUT;
    }

    for (int o = inputs; o < inputs + outputs; o++)
    {
        neuronTypes[o] = OUTPUT;
    }

    for (int i = 0; i < inputs; i++)
    {
        for (int o = inputs; o < inputs + outputs; o++)
        {
            AddSynapse(i, o, 0.0);
        }
    }
}

std::vector<double> NeuralNetwork::Outputs(std::vector<double> inputs)
{
    for (int i = 0; i < inputs.size(); i++)
    {
        inputValues[i] = inputs[i];
    }

    std::vector<double> outputs = std::vector<double>();

    for (int n = 0; n < neuronTypes.size(); n++)
    {
        if (neuronTypes[n] == OUTPUT)
        {
            outputs.push_back(NeuronValue(n));
        }
    }

    return outputs;
}

void NeuralNetwork::AddSynapse(int input, int output, double weight)
{
    synapses.push_back(Synapse(input, output, weight));
}

double NeuralNetwork::NeuronValue(int neuron)
{
    // Base case. If we've got an input, return the input value.
    if (neuronTypes[neuron] == INPUT)
    {
        return inputValues[neuron];
    }

    // First, find all synapses for which the end neuron is this one.
    std::vector<Synapse> inputSynapses = std::vector<Synapse>();

    for (int i = 0; i < synapses.size(); i++)
    {
        if (synapses[i].End() == neuron)
        {
            inputSynapses.push_back(synapses[i]);
        }
    }

    // Now that we know what our inputs are, we can calculate a weighted sum.
    double weightedSum = 0.0;

    for (int i = 0; i < inputSynapses.size(); i++)
    {
        weightedSum += (inputSynapses[i].Weight() * NeuronValue(inputSynapses[i].Start()));
    }

    // The value of the neuron is the weighted sum passed through a sigmoid function.
    // Here we use a fast approximation.
    return weightedSum / (1 + fabs(weightedSum));
}
