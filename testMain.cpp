#include <iostream>
#include "NNetworks.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <map>

//g++ -c NNetworks.cpp -o NNetworks.o
//g++ testMain.cpp NNetworks.o -o main

double teste(double x)
{
	if (x < -5) {
		return 0;
	}
	if (x < 5) {
		return 1;
	}
	return 2;
}

double test_network(Network network, double bounds)
{
	//printf("entrou no test network\t");
	int seed = getCurrentTimeInSeconds();
	std::srand(seed);
	double input[2] = { 1, 0 }, x;
	int counter = 0;
	for (int i = 0; i < 10; i++) {
		x = -1 * bounds +
			(static_cast<double>(std::rand()) / RAND_MAX) * 2 * bounds;
		input[1] = x;
		if (network.runSoftmax(input) == teste(x)) {
			counter++;
		}
	}
	return counter;
}

int main(int argc, char *argv[])
{
	int layers[3] = { 2, 3, 3 };
	std::srand((int)getCurrentTimeInSeconds());
	Network *population; // = Network(2, 3, layers);
	Network *next_gen;
	//network.randomize(rand());
	int n_generations = 30, pop_size = 20, cut = 0.6 * pop_size;
	population = (Network *)malloc(pop_size * sizeof(Network));
	next_gen = (Network *)malloc(cut * sizeof(Network));
	for (int i = 0; i < pop_size; i++) {
		population[i] = Network(3, 2, layers);
		population[i].randomize(8, 1);
	}
	for (int i = 0; i < cut; i++) {
		next_gen[i] = Network();
	}

	int chromosome = NEURONS, mode = SPLICING_HALF, seed;
	double mutation_range = 0.8, mutation_chance = 0.5;
	double output_best = -1000, best_prev = 0, improvement;
	std::vector<std::pair<int, double> > pop_outputs(pop_size);
	std::pair<int, double> temp;
	Network best_of_all = Network(), best_generation = Network();
	for (int i = 0; i < n_generations; i++) {
		std::cout << "tamanho do vetor: " << pop_outputs.size() << "\n";
		std::cout << "output da geracao " << i << ":\n\t";
		for (int j = 0; j < pop_size; j++) {
			temp.first = j;
			temp.second = test_network(population[j], 10);
			std::cout << j << " - " << temp.second << ", ";
			pop_outputs.at(j) = temp;
		}
		std::sort(pop_outputs.begin(), pop_outputs.end(),
				  [](const auto &lhs, const auto &rhs) {
					  return lhs.second > rhs.second;
				  });
		improvement = (pop_outputs.at(0).second -
					   best_prev); // improvement *= improvement;
		best_prev = pop_outputs.at(0).second;
		std::cout << "\nbest dessa geracao: " << pop_outputs.at(0).second
				  << " improvement: " << improvement << "\n";
		if (pop_outputs.at(0).second > output_best) {
			output_best = pop_outputs.at(0).second;
			best_of_all.copyNetwork(population[pop_outputs.at(0).first]);
		}
		next_gen[0].copyNetwork(
			reproduce(population[pop_outputs.at(0).first],
					  population[pop_outputs.at(1).first], chromosome, mode,
					  true, rand(), mutation_range, mutation_chance, NULL));
		int p1, p2;
		for (int j = 1; j < cut; j++) {
			p1 = rand() % cut;
			p2 = rand() % cut;
			printf("cruzando %d com %d\n", p1, p2);
			next_gen[j].copyNetwork(reproduce(
				population[pop_outputs.at(p1).first],
				population[pop_outputs.at(p2).first], chromosome, mode, true,
				rand(), mutation_range, mutation_chance, NULL));
		}
		for (int j = 0; j < cut; j++) {
			population[j].copyNetwork(next_gen[j]);
		}
		for (int j = cut + 1; j < pop_size; j++) {
			population[j].randomize(i + j);
		}
		/*for(int j = 0; j < pop_size; j++)
            population[j] = reproduceAndKillParents(&population[best_local_index], &population[j], 1, chromosome, mode, true, rand(), mutation_range, mutation_chance, NULL);
        */
		printf("\n");
	}
	std::cout << "Output best_of_all: " << output_best << "\n";

	for (int i = 0; i < pop_size; i++) {
		population[i].killNetwork();
	}
	free(population);
	return 0;
}