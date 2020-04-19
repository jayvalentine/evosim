#include "neuralnetwork.h"

void Synapse::ChangeWeight(double newWeight)
{
    weight = newWeight;
}

void Synapse::ScaleWeight(double factor)
{
    ChangeWeight(weight * factor);
}

NeuralNetwork::NeuralNetwork(int inputs, int outputs, int hidden)
{
    neuronTypes = std::vector<NeuronType>();
    inputValues = std::vector<double>();

    inputValues.resize(inputs);

    for (int i = 0; i < inputs; i++)
    {
        neuronTypes.push_back(INPUT);
    }

    for (int o = inputs; o < inputs + outputs; o++)
    {
        neuronTypes.push_back(OUTPUT);
    }

    for (int h = inputs + outputs; h < inputs + outputs + hidden; h++)
    {
        neuronTypes.push_back(HIDDEN);
    }

    inputSize = inputs;
    outputSize = outputs;
}

NeuralNetwork::~NeuralNetwork()
{
    // Delete synapse instances.
    for (int i = 0; i < synapses.size(); i++) delete synapses[i];
}

void NeuralNetwork::OutputValues(double * inputs, double * outputs)
{
    for (int i = 0; i < inputSize; i++)
    {
        inputValues[i] = inputs[i];
    }

    unsigned int o = 0;
    for (int n = 0; n < neuronTypes.size(); n++)
    {
        if (neuronTypes[n] == OUTPUT)
        {
            outputs[o] = NeuronValue(n);
            o++;
        }
    }
}

double NeuralNetwork::NeuronValue(int neuron)
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
    double neuronValue = weightedSum / (1 + fabs(weightedSum));

    return neuronValue;
}

void NeuralNetwork::AddSynapse(int start, int end, double weight)
{
    synapses.push_back(new Synapse(start, end, weight));
}

// Add a hidden neuron on a given synapse.
// The synapse must be between an input neuron and an output one -
// i.e. only one hidden layer is allowed.
void NeuralNetwork::AddHiddenNeuron(int synapseIndex)
{
    Synapse * synapse = synapses[synapseIndex];

    // Get the start and end neuron for the synapse.
    int input = synapse->Start();
    int output = synapse->End();

    // Synapse weight. The two new synapses we create will have this same weight.
    double weight = synapse->Weight();

    // Add the hidden neuron.
    neuronTypes.push_back(HIDDEN);

    // Index of the new neuron.
    int hiddenIndex = neuronTypes.size() - 1;

    // Delete the old synapse and replace with a new one.
    delete synapse;
    synapses[synapseIndex] = new Synapse(input, hiddenIndex, weight);

    // Add a new synapse from the hidden neuron to the end one.
    synapses.push_back(new Synapse(hiddenIndex, output, weight));

    // And we're done!
}

NeuralNetwork::NeuronType NeuralNetwork::Type(int neuron)
{
    if (neuron >= neuronTypes.size()) printf("Invalid neuron: %d for size: %lu\n", neuron, neuronTypes.size());
    return neuronTypes[neuron];
}

std::vector<int> NeuralNetwork::Inputs(void)
{
    std::vector<int> inputs = std::vector<int>();

    for (int i = 0; i < neuronTypes.size(); i++)
    {
        if (neuronTypes[i] == INPUT) inputs.push_back(i);
    }

    return inputs;
}

std::vector<int> NeuralNetwork::Outputs(void)
{
    std::vector<int> outputs = std::vector<int>();

    for (int i = 0; i < neuronTypes.size(); i++)
    {
        if (neuronTypes[i] == OUTPUT) outputs.push_back(i);
    }

    return outputs;
}

std::vector<int> NeuralNetwork::Hidden(void)
{
    std::vector<int> hidden = std::vector<int>();

    for (int i = 0; i < neuronTypes.size(); i++)
    {
        if (neuronTypes[i] == HIDDEN) hidden.push_back(i);
    }

    return hidden;
}

std::vector<Synapse *> NeuralNetwork::Synapses(void)
{
    return synapses;
}
