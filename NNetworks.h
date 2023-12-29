#ifndef NN_H
#define NN_H

#define NEURONS 1
#define LAYERS 2

#define AVERAGE 0
#define SPLICING_HALF 1
#define SPLICING_RAND 2

#define FIRST_PARENT 0
#define SECOND_PARENT 1
#define BOTH_PARENTS 2

//g++ -c NNetworks.cpp -o NNetworks.o
class Neuron{
    private:
        int n_factors;
        unsigned short int layer;
        float*weights;
        float last_output;
        float midpoint;
        float default_mutation;
        //TODO: funcao de ativacao como parametro
    public:
        bool sigmoid;
        //bool hyperTan;
        Neuron();
        Neuron(int n_factors, int layer);
        Neuron(int n_factors, int layer, float*weights, bool sigmoid, float midpoint);
        Neuron(int n_factors, int layer, bool rand, int seed, bool sigmoid, float midpoint);
        void setDefaultMutation(float newMutation);
        void setWeights(float*new_weights);
        int getNFactors();
        int getLayer();
        float getFactor(int index);
        float getLastResult();
        float*copyFactors();
        float output(float*inputs);
        void randomize(int seed);
        void randomize(int seed, float*ranges);
        void randomize(int seed, float range);
        void mutate(float mutationRange, int seed);
        void mutate(float mutation_chance, int seed, bool trash);
        void mutate(float mutationRange, int seed, bool*values_to_mutate);
        void copyNeuron(Neuron new_neuron);
        void printNeuron();
        void freeNeuron();
};

class Network{
    private:
        //TODO: implementar tudo com vector e parar de bater cabeca com pointer
        Neuron**layers;
        int n_layers;
        int*n_neurons_per_layer;
        int n_inputs;
    public:
        Network();
        Network(int n_layers, int n_inputs, int*n_neurons_per_layer);
        Network(int n_layers, int n_inputs, int*n_neurons_per_layer, bool initialize_rand, int seed);
        int getNLayers();
        int getNInputs();
        Neuron getNeuron(int i, int j);
        int NNeuronsInLayerN(int layer);
        void copyNeuron(int i, int j, Neuron new_neuron);
        void copyNetwork(Network &original);
        float* runNetwork(float*input);
        float* runNetwork(float*input, bool softmax);
        int runSoftmax(float*input, int default_decision);
        int runSoftmax(float*input);
        int softmaxLayer(float*input, int layer);
        void randomize(int seed);
        void randomize(int seed, float range);
        //TODO: implementar reproducao assexuada
        //TODO: melhorar a mutacao implementando mutacao padrao
        void killNetwork();
};

Network reproduce(Network P1, Network P2, int chromosome, int mode, bool mutate, int seed, float mutation_range, float mutation_chance);
Network reproduce(Network p1, Network P2, int chromosome, int mode);
Network reproduceAndKillParents(Network *P1, Network *P2, int which, int chromosome, int mode);
Network reproduceAndKillParents(Network *P1, Network *P2, int which, int chromosome, int mode, bool mutate, int seed, float mutation_range, float mutation_chance, bool*values_to_mutate);
float getCurrentTimeInSeconds();
//Network copyNetwork(Network original);

#endif
