#include "NNetworks.h"
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <random>

float *copy(float *origin, int size);

//neuron functions
Neuron::Neuron()
{
	this->n_factors = -1;
	this->layer = -1;
	this->weights = NULL;
	this->sigmoid = false;
}

Neuron::Neuron(int n_factors, int layer)
{
	this->n_factors = n_factors;
	this->weights = (float *)malloc(n_factors * sizeof(float));
	this->layer = layer;
	this->sigmoid = true;
	this->midpoint = 0;
	this->default_mutation = 0.05;
}

Neuron::Neuron(int n_factors, int layer, float *weights, bool sigmoid,
			   float midpoint)
{
	this->n_factors = n_factors;
	this->weights = (float *)malloc(n_factors * sizeof(float));
	this->layer = layer;
	this->setWeights(weights);
	this->sigmoid = sigmoid;
	this->midpoint = midpoint;
	this->default_mutation = 0.05;
}

Neuron::Neuron(int n_factors, int layer, bool rand, int seed, bool sigmoid,
			   float midpoint)
{
	this->n_factors = n_factors;
	this->weights = (float *)malloc(n_factors * sizeof(float));
	this->layer = layer;
	if (rand) {
		this->randomize(seed);
	}
	this->sigmoid = sigmoid;
	this->midpoint = midpoint;
	this->default_mutation = 0.05;
}

void Neuron::setDefaultMutation(float newMutation)
{
	this->default_mutation = newMutation;
}

void Neuron::setWeights(float *new_weights)
{
	for (int i = 0; i < this->n_factors; i++) {
		this->weights[i] = new_weights[i];
	}
}

int Neuron::getNFactors()
{
	return this->n_factors;
}

int Neuron::getLayer()
{
	return this->layer;
}

float Neuron::getFactor(int index)
{
	if (index >= this->n_factors) {
		return 0;
	}
	return this->weights[index];
}

float Neuron::getLastResult()
{
	return this->last_output;
}

float *Neuron::copyFactors()
{
	float *copy = (float *)malloc(this->n_factors * sizeof(float));
	for (int i = 0; i < this->n_factors; i++) {
		copy[i] = this->weights[i];
	}
	return copy;
}

bool Network::validateNetwork(){
	for(int i = 0; i < this->n_layers; i++){
		for(int j = 0; j < this->n_neurons_per_layer[i]; j++){
			for(int k = 0; k < this->layers[i][j].getNFactors(); k++){
				if(std::isnan(this->layers[i][j].getFactor(k))){
					printf("encontrado um nan no fator %d do neuronio %d na layer %d.\n", k, j, i);
					return true;
				}
			}
		}
	}
	return false;
}

float Neuron::output(float *inputs)
{
	float output = weights[0]; //bias
	for (int i = 1; i < this->n_factors; i++) {
		output += this->weights[i] * inputs[i - 1];
	}
	if (this->sigmoid) {
		if(output > 0){
			output = 1 / (1 + exp(-1 * output + this->midpoint));
		}
		else{
			float exp_x = exp(output+this->midpoint);
			output = exp_x/(1+exp_x);
		}
	}
	this->last_output = output;
	return output;
}

void Neuron::randomize(int seed)
{
	std::srand(seed);
	for (int i = 0; i < this->n_factors; i++) {
		this->weights[i] =
			(static_cast<float>(std::rand()) / RAND_MAX * 2.0 - 1.0);
	}
}

void Neuron::randomize(int seed, float *ranges)
{
	std::mt19937 generator(seed); // Mersenne Twister 19937 generator

    for (int i = 0; i < this->n_factors; i++) {
		std::uniform_real_distribution<float> distribution(-ranges[i], ranges[i]);
        this->weights[i] = distribution(generator);
    }
}

void Neuron::randomize(int seed, float range)
{
    std::mt19937 generator(seed); // Mersenne Twister 19937 generator
    std::uniform_real_distribution<float> distribution(-range, range);

    for (int i = 0; i < this->n_factors; i++) {
        this->weights[i] = distribution(generator);
    }
}

void Neuron::randomize(float range, float discrete_step)
{
    //int randomInt = distribution(generator);
	float n_steps = 1*range/discrete_step;
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution((int)-n_steps, (int)n_steps);

    for (int i = 0; i < this->n_factors; i++) {
        this->weights[i] *= (1+distribution(generator)*discrete_step);
    }
}

#include<unistd.h>

void Neuron::mutate(float mutationRange, int seed, bool *values_to_mutate)
{
	std::mt19937 generator(seed); // Mersenne Twister 19937 generator
    std::uniform_real_distribution<float> distribution(-mutationRange, mutationRange);

    for (int i = 0; i < this->n_factors; i++) {
        // std::cout << "value " << i << " before mutating: " << this->weights[i] << "\n";
        if (values_to_mutate[i]) {
            // if the value is too low, regular mutation won't get it exactly to 0, or change its sign
            if (std::abs(this->weights[i]) < 0.1) {
                // if it's exactly 0, set it to a random value
                if (this->weights[i] == 0) {
                    this->weights[i] = distribution(generator);
                } else {
                    // set it to 0 or invert its sign
                    this->weights[i] = (std::rand() % 2 == 0) ? 0.0 : -this->weights[i];
                }
            } else {
                float mutation = distribution(generator);
                this->weights[i] *= (1 + mutation);
            }
            // std::cout << "value " << i << " after mutating: " << this->weights[i] << "\n";
        }
    }
}


void Neuron::mutate(float mutationRange, int seed)
{
	bool all_true[this->n_factors];
	for (int i = 0; i < this->n_factors; i++) {
		all_true[i] = true;
	}
	this->mutate(mutationRange, seed, all_true);
}

void Neuron::mutate(float mutation_chance, int seed, bool trash)
{
	std::srand(seed);
	int n_mutacoes = this->n_factors * mutation_chance, mutating_weight;
	float mutation;
	for (int i = 0; i < n_mutacoes; i++) {
		mutation = (static_cast<float>(std::rand()) / RAND_MAX * 2 - 1) *
				   this->default_mutation;
		mutating_weight = rand() % this->n_factors;
		this->weights[mutating_weight] += mutation;
	}
}

void Neuron::copyNeuron(Neuron original)
{
	this->freeNeuron();
	this->sigmoid = original.sigmoid;
	this->n_factors = original.getNFactors();
	this->weights = (float *)malloc(this->n_factors * sizeof(float));
	for (int i = 0; i < this->n_factors; i++) {
		this->weights[i] = original.getFactor(i);
		//std::cout<<"copiando o fator "<<i<<" : "<<original.getFactor(i)<<" ";
	}
}

void Neuron::printNeuron()
{
	for (int i = 0; i < this->n_factors; i++) {
		std::cout << this->weights[i] << " ";
	}
	printf("\n");
}

void Neuron::freeNeuron()
{
	if (this->weights != NULL) {
		free(this->weights);
		this->n_factors = -1;
	}
}

//network functions

Network::Network()
{
	this->n_layers = - 1;
	this->n_inputs = 0;
	this->n_neurons_per_layer = NULL;
	this->layers = NULL;
	//this->softmax = false;
}

Network::Network(int n_layers, int n_inputs, int *n_neurons_per_layer)
{
	this->n_layers = n_layers;
	this->n_inputs = n_inputs;
	this->layers = (Neuron **)malloc(n_layers * sizeof(Neuron *));
	this->n_neurons_per_layer = (int *)malloc(n_layers * sizeof(int));
	int layer_n_inputs = n_inputs;
	for (int i = 0; i < n_layers; i++) {
		this->n_neurons_per_layer[i] = n_neurons_per_layer[i];
		if (i > 0) {
			layer_n_inputs = n_neurons_per_layer[i - 1] + 1;
		}
		this->layers[i] =
			(Neuron *)malloc(n_neurons_per_layer[i] * sizeof(Neuron));
		for (int j = 0; j < n_neurons_per_layer[i]; j++) {
			layers[i][j] = Neuron(layer_n_inputs, j);
		}
	}
}

Network::Network(int n_layers, int n_inputs, int *n_neurons_per_layer,
				 bool initialize_rand, int seed)
	: Network(n_layers, n_inputs, n_neurons_per_layer)
{
	if (initialize_rand) {
		this->randomize(seed);
	}
}

Network::Network(int n_layers, int n_inputs, int *n_neurons_per_layer,
				 bool initialize_rand, float discrete_step)
	: Network(n_layers, n_inputs, n_neurons_per_layer)
{
	if (initialize_rand) {
		this->randomize((float)0.5, discrete_step);
	}
}

int Network::getNLayers()
{
	return this->n_layers;
}

int Network::getNInputs()
{
	return this->n_inputs;
}

Neuron Network::getNeuron(int i, int j)
{
	if (this->n_layers <= i) {
		return Neuron();
	}
	if (this->n_neurons_per_layer[i] <= j) {
		return Neuron();
	}
	return this->layers[i][j];
}

int Network::NNeuronsInLayerN(int layer)
{
	if (layer >= this->n_layers) {
		return -1;
	}
	return this->n_neurons_per_layer[layer];
}

void Network::copyNeuron(int i, int j, Neuron new_neuron)
{
	//this->printNetwork();
	if (this->n_layers <= i) {
		//printf("overflow de layer\n");
		return;
	}
	if (this->n_neurons_per_layer[i] <= j) {
		//printf("overflow de neuronio\n");
		return;
	}
	/*if (new_neuron.getNFactors() != (this->layers[i][j].getNFactors() + 1)) {
		printf("reprovou no numero de fatores\n");
		return;
	}*/
	this->layers[i][j].copyNeuron(new_neuron); // = Neuron(new_neuron);
	//printf("passou de dar assign o novo neuron\n");
}

void Network::copyNetwork(Network &original)
{
	this->killNetwork();
	this->n_layers = original.getNLayers();
	this->n_neurons_per_layer = (int*) malloc(this->n_layers*sizeof(int));
	this->layers = (Neuron**) malloc(this->n_layers*sizeof(Neuron*));
	for(int i = 0; i < this->n_layers; i++){
		this->n_neurons_per_layer[i] = original.NNeuronsInLayerN(i);
		this->layers[i] = (Neuron*) malloc(this->n_neurons_per_layer[i]*sizeof(Neuron*));
		for(int j = 0; j < this->n_neurons_per_layer[i]; j++){
			this->layers[i][j].copyNeuron(original.getNeuron(i, j));
		}
	}
}

float *Network::runNetwork(float *input)
{
	//there's an easy way of doing this with only one dynamically allocated array, using the largest number of inputs. I, however, have not done it this way
	//printf("entrou no runNetwork\n");
	float *local_inputs = copy(input, this->n_inputs);
	float *results;

	for (int i = 0; i < this->n_layers; i++) {
		results = (float *)malloc(this->n_neurons_per_layer[i] * sizeof(float));
		for (int j = 0; j < this->n_neurons_per_layer[i]; j++) {
			results[j] = this->layers[i][j].output(local_inputs);
		}
		free(local_inputs);
		local_inputs = copy(results, this->n_neurons_per_layer[i]);
		free(results);
	}
	//printf("saindo do runNetwork\n");
	return local_inputs;
}

float *Network::runNetwork(float *input, bool softmax)
{
	//printf("no calculo dos indices softmax\n");
	float *results = this->runNetwork(input);
	if (softmax) {
		//printf("entrando no loop\n");
		float sum = 0;
		for (int i = 0; i < this->n_neurons_per_layer[this->n_layers - 1];
			 i++) {
			//std::cout<<"results[i] old = "<<results[i];
			results[i] = exp(results[i]);
			sum += results[i];
			//std::cout<<" results[i] new = "<<results[i]<<" sum = "<<sum<<" ";
		}
		if (sum == 0) {
			return results;
		}
		for (int i = 0; i < this->n_neurons_per_layer[this->n_layers - 1];
			 i++) {
			results[i] /= sum;
		}
	}
	return results;
}

int Network::runSoftmax(float *input, int default_decision)
{
	//printf("no run softmax. Input is:\n  ");
	float *results = this->runNetwork(input, true),
		  max = results[default_decision];
	int index_max = default_decision;
	//printf("\nresults are:\n  ");
	for (int i = 0; i < this->n_neurons_per_layer[this->n_layers - 1]; i++) {
		//std::cout<<results[i]<<" ";
		if (results[i] > max) {
			//printf("foi encontrado um melhor que o default\n");
			max = results[i];
			index_max = i;
		}
	}
	//printf("\n");
	free(results);
	return index_max;
}

int Network::runSoftmax(float *input)
{
	float *results = this->runNetwork(input, true), max = -1;
	int index_max = -1;
	for (int i = 0; i < this->n_neurons_per_layer[this->n_layers - 1]; i++) {
		if (results[i] > max) {
			max = results[i];
			index_max = i;
		}
	}
	free(results);
	return index_max;
}

int Network::softmaxLayer(float *input, int layer)
{
	if (layer < 0 || layer >= this->n_layers) {
		return -1;
	}
	//TODO: rodar a rede parcialmente ao inves de fazer essa gambiarra
	//TODO: tirar o segundo loop e a divisao pela soma e testar se funciona.
	float *results = runNetwork(input),
		  results_i[this->n_neurons_per_layer[layer]];
	free(results);
	float sum = 0, max = -1;
	int index_max = -1;
	for (int i = 0; i < this->n_neurons_per_layer[layer]; i++) {
		results_i[i] = exp(this->getNeuron(layer, i).getLastResult());
		sum += results_i[i];
	}
	for (int i = 0; i < this->n_neurons_per_layer[layer]; i++) {
		results_i[i] = results_i[i] / sum;
		if (max < results_i[i]) {
			max = results_i[i];
			index_max = i;
		}
	}
	return index_max;
}

void Network::randomize(int seed, float range)
{
	srand(seed);
	for (int i = 0; i < this->n_layers; i++) {
		//printf("randomizing in layer %d of %d, com %d neuronios\n", i, this->n_layers, n_neurons_per_layer[i]);
		for (int j = 0; j < this->n_neurons_per_layer[i]; j++) {
			//printf("randomizing neuron %d\n", j);
			this->layers[i][j].randomize(rand(), range);
		}
	}
}

void Network::randomize(float range, float discrete_step)
{
	for (int i = 0; i < this->n_layers; i++) {
		//printf("randomizing in layer %d of %d, com %d neuronios\n", i, this->n_layers, n_neurons_per_layer[i]);
		for (int j = 0; j < this->n_neurons_per_layer[i]; j++) {
			//printf("randomizing neuron %d\n", j);
			this->layers[i][j].randomize(range, discrete_step);
		}
	}
}

void Network::randomize(int seed)
{
	this->randomize(seed, 1);
}

void Network::mutate(int seed, float mutationRange, float mutationChance){
	std::srand(seed);
	int n_neurons = 0;
	for(int i = 0; i < this->n_layers; i++)
		n_neurons+=this->n_neurons_per_layer[i];
	int n_mutations = mutationChance*n_neurons, temp_layer, temp_neuron;
	for(int i = 0; i < n_mutations; i++){
		temp_layer = rand()%this->n_layers;
		temp_neuron = rand()%this->n_neurons_per_layer[temp_layer];
		this->layers[temp_layer][temp_neuron].mutate(mutationRange, rand());
	}
}

void Network::killNetwork()
{
	if (this->layers == NULL || this->n_neurons_per_layer == NULL) {
		return;
	}
	//printf("entrou ");
	for (int i = 0; i < this->n_layers; i++) {
		for (int j = 0; j < this->n_neurons_per_layer[i]; j++) {
			this->layers[i][j].freeNeuron();
		}
		free(this->layers[i]);
	}
	free(this->layers);
	this->layers = NULL;
	free(this->n_neurons_per_layer);
	this->n_neurons_per_layer = NULL;
	//printf("e saiu do kill network\n");
}

//utility functions. Might be related to objects
float getCurrentTimeInSeconds()
{
	auto currentTime = std::chrono::system_clock::now();
	auto timeInSeconds =
		std::chrono::duration_cast<std::chrono::duration<float> >(
			currentTime.time_since_epoch());

	return timeInSeconds.count();
}

float *copy(float *origin, int size)
{
	float *copy = (float *)malloc(size * sizeof(float));
	for (int i = 0; i < size; i++) {
		copy[i] = origin[i];
	}
	return copy;
}


Neuron reproduce(Neuron P1, Neuron P2, int mode)
{
	//printf("entrando no reproduce neuron\n");
	if ((P1.getNFactors() != P2.getNFactors())) {
		//printf("reprovou no numero de fatores\n");
		return Neuron();
	}
	//printf("passou na validação do reproduce neuron\n");
	int n_factors = P1.getNFactors(); //, layer = P1.getLayer();
	Neuron child = Neuron(n_factors, P1.getLayer());
	float new_weights[n_factors];
	switch (mode) {
	case AVERAGE:
		for (int i = 0; i < n_factors; i++) {
			new_weights[i] = (P1.getFactor(i) + P2.getFactor(i)) / 2;
		}
		break;
	case SPLICING_HALF:
		for (int i = 0; i < n_factors / 2; i++) {
			new_weights[i] = P1.getFactor(i);
		}
		for (int i = n_factors / 2; i < n_factors; i++) {
			new_weights[i] = P2.getFactor(i);
		}
		break;
	case SPLICING_RAND:
		int coin;
		for (int i = 0; i < n_factors; i++) {
			coin = rand() % 2;
			if (coin) {
				new_weights[i] = P1.getFactor(i);
			} else {
				new_weights[i] = P2.getFactor(i);
			}
		}
	}
	child.setWeights(new_weights);
	return child;
}

Network reproduce(Network P1, Network P2, int chromosome, int mode, bool mutate,
				  int seed, float mutation_range, float mutation_chance)
{
	//printf("entrou no reproduce de networks\n");
	if ((P1.getNLayers() != P2.getNLayers()) ||
		(P1.getNInputs() != P2.getNInputs())) {
		return Network();
	}
	int n_layers = P1.getNLayers();
	int n_neurons_per_layer[n_layers];
	for (int i = 0; i < n_layers; i++) {
		if (P1.NNeuronsInLayerN(i) != P2.NNeuronsInLayerN(i)) {
			return Network();
		} else {
			n_neurons_per_layer[i] = P1.NNeuronsInLayerN(i);
		}
	}
	//printf("passou da validacao do reproduce\n");
	Network child(n_layers, P1.getNInputs(), n_neurons_per_layer);
	Neuron temp;
	int coin;
	float die;
	switch (chromosome) {
	case NEURONS:
		for (int i = 0; i < n_layers; i++) {
			for (int j = 0; j < n_neurons_per_layer[i]; j++) {
				//printf("gerando neuronio %d, %d do filho\n", i, j);
				temp = reproduce(P1.getNeuron(i, j), P2.getNeuron(i, j), mode);
				//printf("n_factors de temp is %d\n", temp.getNFactors());
				if (mutate) {
					die = (static_cast<float>(std::rand()) / RAND_MAX);
					//printf("passou do cast the die\n");
					if (die < mutation_chance) {
						temp.mutate(mutation_range, mutation_chance);
					}
				}
				//printf("passou da mutacao\n");
				child.copyNeuron(i, j, temp);
				temp.freeNeuron();
			}
		}
		break;
	case LAYERS:
		if (mode != SPLICING_HALF && mode != SPLICING_RAND) {
			return child;
		}
		for (int i = 0; i < n_layers; i++) {
			if (mode == SPLICING_HALF) {
				for (int j = 0; j < n_neurons_per_layer[i] / 2; j++) {
					temp = P1.getNeuron(i, j);
					if (mutate) {
						temp.mutate(mutation_range, seed);
					}
					child.copyNeuron(i, j, temp);
				}
				for (int j = n_neurons_per_layer[i] / 2;
					 j < n_neurons_per_layer[i]; j++) {
					temp = P2.getNeuron(i, j);
					if (mutate) {
						temp.mutate(mutation_range, seed);
					}
					child.copyNeuron(i, j, temp);
				}
			} else { //if(mode == SPLICING_RAND){}
				for (int j = 0; j < n_neurons_per_layer[i]; j++) {
					coin = rand() % 2;
					if (coin) {
						temp = P1.getNeuron(i, j);
					} else {
						temp = P2.getNeuron(i, j);
					}
					if (mutate) {
						temp.mutate(mutation_range, seed);
					}
					child.copyNeuron(i, j, temp);
				}
			}
		}
		break;
	}
	child.validateNetwork();
	return child;
}

Network reproduce(Network P1, Network P2, int chromosome, int mode)
{
	return reproduce(P1, P2, chromosome, mode, false, -1, 0, 0);
}

Network reproduceAndKillParents(Network *P1, Network *P2, int which,
								int chromosome, int mode, bool mutate, int seed,
								float mutation_range, float mutation_chance)
{
	//TODO:checar validade de P1 e P2, modificar pra nao matar ninguem se child for invalido
	Network child = reproduce(*P1, *P2, chromosome, mode, mutate, seed,
							  mutation_range, mutation_chance);
	switch (which) {
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
