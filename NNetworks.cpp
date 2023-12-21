#include"NNetworks.h"
#include <cstdio>
#include<iostream>
#include <cstdlib>
#include <cmath>
#include<chrono>

double* copy(double*origin, int size);

//neuron functions
Neuron::Neuron(){
    this->n_factors = -1;
    this->layer = -1;
    this->weights = NULL;
    this->sigmoid = false;
}
Neuron::Neuron(int n_factors, int layer){
    this->n_factors = n_factors;
    this->weights = (double*) malloc(n_factors*sizeof(double));
    this->layer = layer;
    this->sigmoid = true;
    this->midpoint = 0;
}
Neuron::Neuron(int n_factors, int layer, double*weights, bool sigmoid, double midpoint){
    this->n_factors = n_factors;
    this->weights = (double*) malloc(n_factors*sizeof(double));
    this->layer = layer;
    this->setWeights(weights);
    this->sigmoid = sigmoid;
    this->midpoint = midpoint;
}
Neuron::Neuron(int n_factors, int layer, bool rand, int seed, bool sigmoid, double midpoint){
    this->n_factors = n_factors;
    this->weights = (double*) malloc(n_factors*sizeof(double));
    this->layer = layer;
    if(rand)
        this->randomize(seed);
    this->sigmoid = sigmoid;
    this->midpoint = midpoint;
}
void Neuron::setWeights(double*new_weights){
    for(int i = 0; i < this->n_factors; i++)
        this->weights[i] = new_weights[i];
}
int Neuron::getNFactors(){
    return this->n_factors;
}
int Neuron::getLayer(){
    return this->layer;
}
double Neuron::getFactor(int index){
    if(index >= this->n_factors)
        return 0;
    return this->weights[index];
}
double Neuron::getLastResult(){
    return this->last_output;
}
double*Neuron::copyFactors(){
    double*copy = (double*)malloc(this->n_factors*sizeof(double));
    for(int i = 0; i < this->n_factors; i++)
        copy[i] = this->weights[i];
    return copy;
}
double Neuron::output(double*inputs){
    double output = weights[0];//bias
    for(int i = 1; i < this->n_factors; i++){
        output+=this->weights[i]*inputs[i-1];
    }
    if(this->sigmoid){
        output = 1/(1+exp(-1*output+this->midpoint));
    }
    this->last_output=output;
    //std::cout<<"output is "<<output<<"\n";
    return output;
}
void Neuron::randomize(int seed){
    std::srand(seed);
    for(int i = 0; i < this->n_factors; i++)
        this->weights[i] = (static_cast<double>(std::rand()) / RAND_MAX * 2.0 - 1.0);
}
void Neuron::randomize(int seed, double*ranges){
    std::srand(seed);
    for(int i = 0; i < this->n_factors; i++){
        this->weights[i] = (static_cast<double>(std::rand()) / RAND_MAX * 2.0*ranges[i] - 1.0*ranges[i]);
    }
}
void Neuron::randomize(int seed, double range){
    std::srand(seed);
    for(int i = 0; i < this->n_factors; i++){
        this->weights[i] = (static_cast<double>(std::rand()) / RAND_MAX * 2.0*range - 1.0*range);
    }
}
void Neuron::mutate(double mutationRange, int seed, bool*values_to_mutate){
    std::srand(seed);
    double mutation;
    int coin;
    for(int i = 0; i < this->n_factors; i++){
        //std::cout<<"value "<<i<<" before mutating: "<<this->weights[i]<<"\n";
        if(values_to_mutate[i]){
            //if the value is too low, regular mutation won't get it exactly to 0, or change it's signal
            if(this->weights[i] < 0.1 && this->weights[i] > -0.1){
                coin = std::rand()%2;
                //and if it's exactly 0, regular mutation won't get it to anything
                if(this->weights[i] == 0){
                    if(coin){
                        coin = std::rand()%2;
                        if(coin)
                            this->weights[i] = mutationRange;
                        else
                            this->weights[i] = -1*mutationRange;
                    }
                }
                else{
                    if(coin)
                        this->weights[i] = 0;
                    else
                        this->weights[i] = -1*this->weights[i];
                }
            }
            mutation = (static_cast<double>(std::rand()) / RAND_MAX * 2*mutationRange - mutationRange);
            this->weights[i]*=(1+mutation);
            //std::cout<<"value "<<i<<" after mutating: "<<this->weights[i]<<"\n";
        }
    }
}
void Neuron::mutate(double mutationRange, int seed){
    bool all_true[this->n_factors];
    for(int i = 0; i < this->n_factors; i++)
        all_true[i] = true;
    this->mutate(mutationRange, seed, all_true);
}
void Neuron::copyNeuron(Neuron new_neuron){
    this->freeNeuron();
    this->sigmoid = new_neuron.sigmoid;
    this->n_factors = new_neuron.getNFactors();
    this->weights = (double*) malloc(this->n_factors*sizeof(double));
    for(int i = 0; i < this->n_factors; i++)
        this->weights[i] = new_neuron.getFactor(i);
}
void Neuron::printNeuron(){
    for(int i = 0; i < this->n_factors; i++)
        std::cout<<this->weights[i]<<" ";
    printf("\n");
}
void Neuron::freeNeuron(){
    if(this->weights != NULL){
        free(this->weights);
        this->n_factors = -1;
    }
}

//network functions

Network::Network(){
    this->n_layers -1;
    this->n_inputs = 0;
    this->n_neurons_per_layer = NULL;
    this->layers = NULL;
    //this->softmax = false;
}
Network::Network(int n_layers, int n_inputs, int*n_neurons_per_layer){
    this->n_layers = n_layers;
    this->n_inputs = n_inputs;
    this->layers = (Neuron**)malloc(n_layers*sizeof(Neuron*));
    this->n_neurons_per_layer = (int*)malloc(n_layers*sizeof(int));
    int layer_n_inputs = n_inputs;
    for(int i = 0; i < n_layers; i++){
        this->n_neurons_per_layer[i] = n_neurons_per_layer[i];
        if(i > 0){
            layer_n_inputs = n_neurons_per_layer[i-1]+1;
        }
        this->layers[i] = (Neuron*)malloc(n_neurons_per_layer[i]*sizeof(Neuron));
        for(int j = 0; j < n_neurons_per_layer[i]; j++){
            layers[i][j] = Neuron(layer_n_inputs, j);
        }
    }
}
Network::Network(int n_layers, int n_inputs, int*n_neurons_per_layer, bool initialize_rand, int seed) : Network(n_layers, n_inputs, n_neurons_per_layer) {
    if(initialize_rand)
        this->randomize(seed);
}
int Network::getNLayers(){
    return this->n_layers;
}
int Network::getNInputs(){
    return this->n_inputs;
}
Neuron Network::getNeuron(int i, int j){
    if(this->n_layers <= i)
        return Neuron();
    if(this->n_neurons_per_layer[i] <= j)
        return Neuron();
    return this->layers[i][j];
}
int Network::NNeuronsInLayerN(int layer){
    if(layer >= this->n_layers)
        return -1;
    return this->n_neurons_per_layer[layer];
}
void Network::copyNeuron(int i, int j, Neuron new_neuron){
    //printf("entrou no copy neuron\n");
    //this->printNetwork();
    if(this->n_layers <= i)
        return;
    if(this->n_neurons_per_layer[i] <= j)
        return;
    if(new_neuron.getNFactors() != (this->layers[i][j].getNFactors()+1))
        return;
    //printf("passou da validacao\n");
    this->layers[i][j].copyNeuron(new_neuron);// = Neuron(new_neuron);
    //printf("passou de dar assign o novo neuron\n");
}
void Network::copyNetwork(Network &original){
	printf("entrou no copyNetwork\n");
	printf("numero de camadas: %d\n", original.getNLayers());
	printf("verif entrda %d: %d\n", 0, original.NNeuronsInLayerN(0));
    this->killNetwork();
	printf("verif kill %d: %d\n", 0, original.NNeuronsInLayerN(0));
    int n_layers = original.getNLayers();
    this->n_layers = n_layers;
	printf("numero de neuronios na camada %d: %d\n", 0, original.NNeuronsInLayerN(0));
    this->n_inputs = original.getNInputs();
    this->layers = (Neuron**)malloc(n_layers*sizeof(Neuron*));
    this->n_neurons_per_layer = (int*)malloc(n_layers*sizeof(int));
    int layer_n_inputs = original.getNInputs();
    for(int i = 0; i < n_layers; i++){
		printf("entrou no primeiro loop\n");
        this->n_neurons_per_layer[i] = original.NNeuronsInLayerN(i);
		printf("numero de neuronios na camada %d: %d\n", i, original.NNeuronsInLayerN(i));
        if(i > 0){
            layer_n_inputs = original.NNeuronsInLayerN(i-1);
        }
        this->layers[i] = (Neuron*)malloc(original.NNeuronsInLayerN(i)*sizeof(Neuron));
        for(int j = 0; j < original.NNeuronsInLayerN(i); j++){
			printf("copiando neuronio %d da camada %d\n", j, i);
            layers[i][j] = Neuron();
            layers[i][j].copyNeuron(original.getNeuron(i, j));
        }
    }
}
double* Network::runNetwork(double*input){
    //there's an easy way of doing this with only one dynamically allocated array, using the largest number of inputs. I, however, have not done it this way
    //printf("entrou no runNetwork\n");
    double *local_inputs = copy(input, this->n_inputs); double *results;

    for(int i = 0; i < this->n_layers; i++){
    results = (double*) malloc(this->n_neurons_per_layer[i]*sizeof(double));
    for(int j = 0; j < this->n_neurons_per_layer[i]; j++){
        results[j] = this->layers[i][j].output(local_inputs);
    }
    free(local_inputs);
    local_inputs = copy(results, this->n_neurons_per_layer[i]);
    free(results);
}
    //printf("saindo do runNetwork\n");
    return local_inputs;
}
double* Network::runNetwork(double*input, bool softmax){
    //printf("no calculo dos indices softmax\n");
    double*results = this->runNetwork(input);
    if(softmax){
        //printf("entrando no loop\n");
        double sum = 0;
        for(int i = 0; i < this->n_neurons_per_layer[this->n_layers-1]; i++){
            //std::cout<<"results[i] old = "<<results[i];
            results[i] = exp(results[i]);
            sum+=results[i];
            //std::cout<<" results[i] new = "<<results[i]<<" sum = "<<sum<<" ";
        }
        if(sum == 0)
            return results;
        for(int i = 0; i < this->n_neurons_per_layer[this->n_layers-1]; i++)
            results[i] /= sum;
    }
    return results;
}
int Network::runSoftmax(double*input, int default_decision){
    //printf("no run softmax. Input is:\n  ");
    double*results = this->runNetwork(input, true), max = results[default_decision];
    int index_max = default_decision;
    //printf("\nresults are:\n  ");
    for(int i = 0; i < this->n_neurons_per_layer[this->n_layers-1]; i++){
        //std::cout<<results[i]<<" ";
        if(results[i] > max){
            max = results[i];
            index_max = i;
        }
    }
    //printf("\n");
    free(results);
    return index_max;
}
int Network::softmaxLayer(double*input, int layer){
    if(layer < 0 || layer >= this->n_layers)
        return -1;
    //TODO: rodar a rede parcialmente ao inves de fazer essa gambiarra
    //TODO: tirar o segundo loop e a divisao pela soma e testar se funciona.
    double *results = runNetwork(input), results_i[this->n_neurons_per_layer[layer]];
    free(results);
    double sum = 0, max = -1;
    int index_max = -1;
    for(int i = 0; i < this->n_neurons_per_layer[layer]; i++){
        results_i[i] = exp(this->getNeuron(layer, i).getLastResult());
        sum+=results_i[i];
    }
    for(int i = 0; i < this->n_neurons_per_layer[layer]; i++){
        results_i[i] = results_i[i]/sum;
        if(max < results_i[i]){
            max = results_i[i];
            index_max = i;
        }
    }
    return index_max;
}
void Network::randomize(int seed, double range){
    srand(seed);
    for(int i = 0; i < this->n_layers; i++){
        //printf("randomizing in layer %d of %d, com %d neuronios\n", i, this->n_layers, n_neurons_per_layer[i]);
        for(int j = 0; j < this->n_neurons_per_layer[i]; j++){
            //printf("randomizing neuron %d\n", j);
            this->layers[i][j].randomize(rand(), range);
        }
    }
}
void Network::randomize(int seed){
    this->randomize(seed, 1);
}
void Network::killNetwork(){
    if(this->layers == NULL || this->n_neurons_per_layer == NULL) {
        return;
	}
	//printf("entrou ");
    for(int i = 0; i < this->n_layers; i++){
        for(int j = 0; j < this->n_neurons_per_layer[i]; j++)
            this->layers[i][j].freeNeuron();
        free(this->layers[i]);
    }
    free(this->layers);
    this->layers = NULL;
    free(this->n_neurons_per_layer);
    this->n_neurons_per_layer = NULL;
    //printf("e saiu do kill network\n");
}

//utility functions. Might be related to objects
double getCurrentTimeInSeconds() {
    auto currentTime = std::chrono::system_clock::now();
    auto timeInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime.time_since_epoch());

    return timeInSeconds.count();
}
double* copy(double*origin, int size){
    double*copy = (double*) malloc(size*sizeof(double));
    for(int i = 0; i < size; i++)
        copy[i] = origin[i];
    return copy;
}
Neuron reproduce(Neuron P1, Neuron P2, int mode){
    if((P1.getNFactors() != P2.getNFactors()) || (P1.getLayer() != P2.getLayer()))
        return Neuron();
    int n_factors = P1.getNFactors();//, layer = P1.getLayer();
    Neuron child = Neuron(n_factors, P1.getLayer());
    double new_weights[n_factors];
    switch (mode)
    {
    case AVERAGE:
        for(int i = 0; i < n_factors; i++)
            new_weights[i] = (P1.getFactor(i)+P2.getFactor(i))/2;
        break;
    case SPLICING_HALF:
        for(int i = 0; i < n_factors/2; i++)
            new_weights[i] = P1.getFactor(i);
        for(int i = n_factors/2; i < n_factors; i++)
            new_weights[i] = P2.getFactor(i);
        break;
    case SPLICING_RAND:
        int coin;
        for(int i = 0; i < n_factors; i++){
            coin = rand()%2;
            if(coin)
                new_weights[i] = P1.getFactor(i);
            else
                new_weights[i] = P2.getFactor(i);
        }
    }
    child.setWeights(new_weights);
    return child;
}
Network reproduce(Network P1, Network P2, int chromosome, int mode, bool mutate, int seed, double mutation_range, double mutation_chance, bool*values_to_mutate){
    //printf("entrou no reproduce de networks\n");
    if((P1.getNLayers() != P2.getNLayers()) || (P1.getNInputs() != P2.getNInputs()))
        return Network();
    int n_layers = P1.getNLayers();
    int n_neurons_per_layer[n_layers];
    for(int i = 0; i < n_layers; i++)
        if(P1.NNeuronsInLayerN(i) != P2.NNeuronsInLayerN(i))
            return Network();
        else
            n_neurons_per_layer[i] = P1.NNeuronsInLayerN(i);
    //printf("passou da validacao do reproduce\n");
    Network child(n_layers, P1.getNInputs(), n_neurons_per_layer);
    Neuron temp;
    int coin;
    double die;
    switch (chromosome)
    {
    case NEURONS:
        for(int i = 0; i < n_layers; i++){
            for(int j = 0; j < n_neurons_per_layer[i]; j++){
                //printf("gerando neuronio %d, %d do filho\n", i, j);
                temp = reproduce(P1.getNeuron(i, j), P2.getNeuron(i, j), mode);
                //printf("passou do reproduce neuron\n");
                if(mutate){
                    die = (static_cast<double>(std::rand()) / RAND_MAX);
                    //printf("passou do cast the die\n");
                    if(die < mutation_chance) {
                        if(values_to_mutate == NULL)
                            temp.mutate(mutation_range, mutation_chance);
                        else
                            temp.mutate(mutation_range, mutation_chance, values_to_mutate);
					}
                }
                //printf("passou da mutacao\n");
                child.copyNeuron(i, j, temp);
                //printf("passou do copyNeuron\n");
                temp.freeNeuron();
                //printf("passou do free no temp\n");
            }
        }
        break;
    case LAYERS:
        if(mode != SPLICING_HALF && mode != SPLICING_RAND)
            return child;
        for(int i = 0; i < n_layers; i++){
            if(mode == SPLICING_HALF){
                for(int j = 0; j < n_neurons_per_layer[i]/2; j++){
                    temp = P1.getNeuron(i, j);
                    if(mutate)
                        temp.mutate(mutation_range, seed);
                    child.copyNeuron(i, j, temp);
                }
                for(int j = n_neurons_per_layer[i]/2; j < n_neurons_per_layer[i]; j++){
                    temp = P2.getNeuron(i, j);
                    if(mutate)
                        temp.mutate(mutation_range, seed);
                    child.copyNeuron(i, j, temp);
                }
            }else{//if(mode == SPLICING_RAND){}
                for(int j = 0; j < n_neurons_per_layer[i]; j++){
                    coin = rand()%2;
                    if(coin)
                        temp = P1.getNeuron(i, j);
                    else
                        temp = P2.getNeuron(i, j);
                    if(mutate)
                        temp.mutate(mutation_range, seed);
                    child.copyNeuron(i, j, temp);
                }
            }
        }
        break;
    }
    //printf("chegou no return\n");
    return child;
}
Network reproduce(Network P1, Network P2, int chromosome, int mode){
    return reproduce(P1, P2, chromosome, mode, false, -1, 0, 0, NULL);
}
Network reproduceAndKillParents(Network *P1, Network *P2, int which, int chromosome, int mode, bool mutate, int seed, double mutation_range, double mutation_chance, bool*values_to_mutate){
    //TODO:checar validade de P1 e P2, modificar pra nao matar ninguem se child for invalido
    Network child = reproduce(*P1, *P2, chromosome, mode, mutate, seed, mutation_range, mutation_chance, values_to_mutate);
    switch (which)
    {
    case 0:
        P1->killNetwork();
        //free(P1);
        break;
    case 1:
        P2->killNetwork();
        //free(P2);
        break;
    default:
        P1->killNetwork();
        //free(P1);
        P2->killNetwork();
        //free(P2);
        break;
    }
    return child;
}

