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
        OUTPUT
    };

    NeuralNetwork(int inputs, int outputs);

    std::vector<double> OutputValues(std::vector<double> inputs);

    void AddSynapse(int input, int output, double weight);

    double NeuronValue(int neuronIndex);

    std::vector<int> Inputs(void);
    std::vector<int> Outputs(void);

    private:

    std::vector<Synapse> synapses;
    std::vector<double> inputValues;
    std::vector<NeuronType> neuronTypes;
};
