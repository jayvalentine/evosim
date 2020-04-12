#include "evolution.h"

void Evolution::AddRandomSynapse(NeuralNetwork * net)
{
    // Get input neurons.
    std::vector<int> inputs = net->Inputs();
    std::vector<int> outputs = net->Outputs();
    std::vector<Synapse *> synapses = net->Synapses();

    bool chosen = false;

    int start;
    int end;

    // Pick a pair of neurons that do not already have a synapse.
    while (!chosen)
    {
        start = Random::Choice<int>(inputs);
        end = Random::Choice<int>(outputs);

        // Determine if there exists a synapse with this pair.
        chosen = true;
        for (int i = 0; i < synapses.size(); i++)
        {
            if (synapses[i]->Start() == start && synapses[i]->End() == end)
            {
                chosen = false;
                break;
            }
        }

        // Determine if there's a hidden neuron connecting these neurons.
        for (int i = 0; i < synapses.size(); i++)
        {
            if (synapses[i]->Start() == start)
            {
                int hidden = synapses[i]->End();

                for (int j = 0; j < synapses.size(); j++)
                {
                    if (synapses[j]->Start() == hidden && synapses[j]->End() == end)
                    {
                        chosen = false;
                        break;
                    }
                }
            }

            if (!chosen) break;
        }
    }

    // At this point we've found a valid pair, so add a new synapse.
    net->AddSynapse(start, end, Random::Double(-1, 1));
}

void Evolution::Mutate(NeuralNetwork * net)
{
    // Probabilities:
    //
    // - Turn a synapse into a hidden neuron: 0.05
    // - Add a new synapse: 0.1
    // - Scale existing synapse weight: 0.25
    // - Change existing synapse weight: 0.6
    double roll = Random::Double(0, 1);

    bool canAddHidden = false;

    std::vector<Synapse *> synapses = net->Synapses();

    for (int i = 0; i < synapses.size(); i++)
    {
        // If a synapse exists between an input and output neuron then we can add a hidden neuron.
        if (net->Type(synapses[i]->Start()) == NeuralNetwork::NeuronType::INPUT
            && net->Type(synapses[i]->End()) == NeuralNetwork::NeuronType::OUTPUT)
        {
            canAddHidden = true;
            break;
        }
    }

    if (roll < 0.05 && canAddHidden)
    {
        std::vector<int> candidates = std::vector<int>();

        for (int i = 0; i < synapses.size(); i++)
        {
            if (net->Type(synapses[i]->Start()) == NeuralNetwork::NeuronType::INPUT
                && net->Type(synapses[i]->End()) == NeuralNetwork::NeuronType::OUTPUT)
            {
                candidates.push_back(i);
            }
        }

        int index = Random::Choice<int>(candidates);

        net->AddHiddenNeuron(index);
    }
    else if (roll < 0.15 && net->Synapses().size() < (net->Inputs().size() * net->Outputs().size()))
    {
        AddRandomSynapse(net);
    }
    else if (roll < 0.4)
    {
        // Do nothing if the network has no synapses.
        if (net->Synapses().size() > 0)
        {
            // Pick a random synapse.
            Synapse * s = Random::Choice<Synapse *>(net->Synapses());

            // Scale its weight.
            s->ScaleWeight(Random::Double(0.8, 1.2));
        }
    }
    else
    {
        // Do nothing if the network has no synapses.
        if (net->Synapses().size() > 0)
        {
            // Pick a random synapse.
            Synapse * s = Random::Choice<Synapse *>(net->Synapses());

            // Change its weight to a value in the range -1, 1.
            s->ChangeWeight(Random::Double(-1.0, 1.0));
        }
    }
}
