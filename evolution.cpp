#include "evolution.h"

void Evolution::Mutate(NeuralNetwork * net)
{
    // Probabilities:
    //
    // - Add a new synapse: 0.5
    // - Scale existing synapse weight: 0.3
    // - Change existing synapse weight: 0.2
    double roll = Random::Double(0, 1);

    if (roll < 0.5 && net->Synapses().size() < (net->Inputs().size() * net->Outputs().size()))
    {
        // Get input neurons.
        std::vector<int> inputs = net->Inputs();
        std::vector<int> outputs = net->Outputs();
        std::vector<Synapse *> synapses = net->Synapses();

        bool chosen = false;

        int start;
        int end;

        // Pick a pair of neurons that do not already have a synapse.
        while (!chosen) {
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
        }

        // At this point we've found a valid pair, so add a new synapse.
        net->AddSynapse(start, end, Random::Double(-1, 1));
    }
    else if (roll < 0.8)
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
