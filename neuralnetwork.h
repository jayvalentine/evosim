#include <vector>
#include <tuple>

#include <math.h>

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

    std::vector<double> Outputs(std::vector<double> inputs);

    void AddSynapse(int input, int output, double weight);

    private:

    double NeuronValue(int neuronIndex);

    std::vector<Synapse> synapses;
    std::vector<double> inputValues;
    std::vector<NeuronType> neuronTypes;
};
