#include <random>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "NNetworks.h"
#include <algorithm>
#include <vector>
#include <utility>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdio_ext.h>

#define ELITIST 0
#define N_BEST 1
#define ASSEXUAL 2

#define windowWidth 800
#define windowHeight 800

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3
char strings[5][7] = { "up", "down", "right", "left" };

#define OCCUPIED 1000

#define MAX_MUTATION_CHANCE 0.2
#define MIN_MUTATION_CHANCE 0.02
#define MAX_MUTATION_RANGE 0.1
#define MIN_MUTATION_RANGE 0.01

int **grid;

//TODO: dar cores aos agentes e colocar os filhos para herdarem as cores dos pais
/*
TODO: garantir que getGridData tá mandando só 5 informações, ao invés das 9 anteriores
	  criar um #define OCCUPIED 1000
	  testar esse OCCUPIED até as coisas funcionarem
	  colocar umas 400 gerações pra o negócio evoluir de verdade
	  colocar as redes neurais e os resultados num arquivo
	  plotar os resultados do arquivo
	  criar uma rede neural a partir do arquivo
*/
typedef struct _agent {
	int line;
	int column;
	Network network;
	int survival_time;
	bool alive;
	int prev_pos;
	int new_pos;
	int n_dodges;
} agent;

agent *population;
int alive_pop;

typedef struct _bean {
	bool type; //linha ou coluna
	int coordinate;
	float counter;
} bean;

bean *beans;

float boundedRand(int seed, float min, float max)
{
	std::srand(seed);
	//(static_cast<float>(std::rand())/RAND_MAX)
	return min + (static_cast<float>(std::rand()) / RAND_MAX) * (max - min);
}

void draw();
void simulation(int);
int gridSize;
int pop_size;
float bean_delay;
int max_beans;
int n_beans = 0;
unsigned int bean_index = 0;
int generation_counter = 0;
int generation_duration;
int n_generations = 0;
int n_best = 0; // number of elements to use on breedNBest
std::vector<std::pair<int, float> > pop_outputs;
std::vector<std::pair<int, float> > pop_alive;
agent bestOfAll;

int mode;
int breeding;

void clearGrid();

int window;
int max_generations;
float *best_score_of_n;
int *best_time_of_n;
int *best_dodges_of_n;
int *alive_to_the_end;
bool running;
int n_inputs = 6, n_layers = 1, n_per_l[1] = { 4 };
int breeding_type = 0;

int slow_down = 10;
void keyboard(unsigned char key, int x, int y);

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	std::srand(getCurrentTimeInSeconds());

	bestOfAll.survival_time = -1;

	std::cout << "insira o tamanho do grid ";
	std::cin >> gridSize;

	std::cout
		<< "Escolha o tipo de cruzamento: ELITIST = 0, N_BEST = 1, ASSEXUAL = 2\n";
	std::cin >> breeding_type;

	std::cout
		<< "insira o número de indivíduos, a duração de uma geração em frames e o numero de geracoes (nessa ordem, separado por epaços)\n";
	std::cin >> pop_size >> generation_duration >> max_generations;

	std::cout
		<< "insira o número de raios que você quer atingindo a população, e o número de frames até um deles matar\n";
	std::cin >> max_beans >> bean_delay;

	std::cout
		<< "insira o modo: AVERAGE = 0, SPLICING_HALF = 1, SPLICING_RAND = 2\n";
	std::cin >> mode;
	//std::cout<<"por fim, insira o modo de cruzamento: N Best = 0, elitist = 1\n";
	//std::cin>>breeding;

	grid = (int **)malloc(gridSize * sizeof(int *));
	for (int i = 0; i < gridSize; i++) {
		grid[i] = (int *)malloc(gridSize * sizeof(int));
		for (int j = 0; j < gridSize; j++) {
			grid[i][j] = 0;
		}
	}
	alive_pop = pop_size;

	//if (breeding_type == N_BEST){
	//	n_best = 0.4*pop_size;
	//}
	if (pop_size < 20 && breeding_type == N_BEST) {
		while (pop_size < 20) {
			std::cout
				<< "População muito pequena, insira um valor maior ou igual a 20\n";
			std::cin >> pop_size;
		}
	} else if (pop_size < 100) {
		n_best = pop_size / 4;
	} else if (pop_size < 1000) {
		n_best = pop_size / 5;
	} else {
		n_best = pop_size / 8;
	}

	population = (agent *)malloc(pop_size * sizeof(agent));
	best_score_of_n = (float *)malloc(max_generations * sizeof(float));
	best_time_of_n = (int *)malloc(max_generations * sizeof(int));
	best_dodges_of_n = (int *)malloc(max_generations * sizeof(int));
	alive_to_the_end = (int *)malloc(max_generations * sizeof(int));
	int temp_x, temp_y;
	for (int i = 0; i < pop_size; i++) {
		do {
			temp_x = rand() % gridSize;
			temp_y = rand() % gridSize;
		} while (grid[temp_x][temp_y] == OCCUPIED);
		population[i].line = temp_x;
		population[i].column = temp_y;
		grid[temp_x][temp_y] = OCCUPIED;
		int seed = rand();
		//printf("seed %d is %d\n", i, seed);
		population[i].network =
			Network(n_layers, n_inputs, n_per_l, true, seed);
		population[i].alive = true;
		population[i].survival_time = 0;
		population[i].prev_pos = 0;
		population[i].n_dodges = 0;
	}
	beans = (bean *)malloc(max_beans * sizeof(bean));

	running = true;
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(120, 120);
	window = glutCreateWindow("simulacao");
	glClearColor(0, 0, 0, 1.0);
	glutDisplayFunc(draw);
	glutTimerFunc(0, simulation, 0);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	printf("saiu do game\n");
	free(beans);
	printf("deu free no beans\n");
	for (int i = 0; i < pop_size; i++) {
		population[i].network.killNetwork();
	}
	free(population);
	printf("deu free na populacao\n");
	for (int i = 0; i < gridSize; i++) {
		free(grid[i]);
	}
	free(grid);
	printf("deu free no grid\n");
	free(best_score_of_n);
	free(best_time_of_n);
	free(best_dodges_of_n);
	free(alive_to_the_end);
	return 0;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 's':
		if (slow_down > 10) {
			slow_down = 10;
		} else {
			slow_down++;
		}
		break;

	case 'f':
		if (slow_down < 1) {
			slow_down = 1;
		} else {
			slow_down++;
		}
	}
}

void checkBest(bool time)
{
	pop_outputs.clear();

	if (time) {
		for (int i = 0; i < pop_size; i++) {
			// if we are considering the agents which are still alive as the best
			// agents, then we must save them in a separate vector first
			if (population[i].alive) {
				pop_alive.push_back(
					std::make_pair(i, ((float)population[i].survival_time *
									   (float)population[i].n_dodges) /
										  (float)generation_duration));
			} else {
				pop_outputs.push_back(
					std::make_pair(i, ((float)population[i].survival_time *
									   (float)population[i].n_dodges) /
										  (float)generation_duration));

				//printf("Conferindo validade do network %d: %d\n", i, population[i].network.NNeuronsInLayerN(0));
			}
		}
		// We will sort both vectors and then add the vector of the agents who
		// are still alive to the beggining of the pop_outputs vector
		std::sort(pop_alive.begin(), pop_alive.end(),
				  [](const auto &lhs, const auto &rhs) {
					  return lhs.second > rhs.second;
				  });

		std::sort(pop_outputs.begin(), pop_outputs.end(),
				  [](const auto &lhs, const auto &rhs) {
					  return lhs.second > rhs.second;
				  });

		for (int i = pop_alive.size() - 1; i >= 0; i--) {
			std::vector<std::pair<int, float> >::iterator it;
			it = pop_outputs.begin();
			pop_outputs.insert(it, pop_alive.at(i));
		}
	} else {
		for (int i = 0; i < pop_size; i++) {
			pop_outputs.push_back(
				std::make_pair(i, ((float)population[i].survival_time *
								   (float)population[i].n_dodges) /
									  (float)generation_duration));
			//printf("Conferindo validade do network %d: %d\n", i, population[i].network.NNeuronsInLayerN(0));
		}

		std::sort(pop_outputs.begin(), pop_outputs.end(),
				  [](const auto &lhs, const auto &rhs) {
					  return lhs.second > rhs.second;
				  });
	}

	float best_score =
		((float)population[pop_outputs.at(0).first].survival_time *
		 (float)population[pop_outputs.at(0).first].n_dodges) /
		(float)generation_duration;

	printf("gen %d: alive = %d, best: time = %d, n dodges = %d, score = %lf \n",
		   n_generations, alive_pop,
		   population[pop_outputs.at(0).first].survival_time,
		   population[pop_outputs.at(0).first].n_dodges, best_score);

	best_score_of_n[n_generations - 1] = best_score;

	best_time_of_n[n_generations - 1] =
		population[pop_outputs.at(0).first].survival_time;

	best_dodges_of_n[n_generations - 1] =
		population[pop_outputs.at(0).first].n_dodges;

	alive_to_the_end[n_generations - 1] = alive_pop;

	//printf("best of all: survival time: %d, n dodges: %d\n", bestOfAll.survival_time);

	if (((float)(bestOfAll.survival_time * bestOfAll.n_dodges) /
		 (float)generation_duration) < best_score) {
		//printf("Entrou best of all\n");
		bestOfAll = population[pop_outputs.at(0).first];
		//printf("popoutputs: %d\n", pop_outputs.at(0).first);
		//printf("sla: %d\n", population[pop_outputs.at(0).first].network.NNeuronsInLayerN(0));
		//bestOfAll.network.copyNetwork(population[pop_outputs.at(0).first].network);
		bestOfAll.network = population[pop_outputs.at(0).first].network;
		//printf("saiu best of all\n");
	}
}

void genocide(int survivor_index)
{
	printf("entrando no genocidio. Index is %d, pop_size is %d\n",
		   survivor_index, pop_size);
	for (int i = 0; i < pop_size; i++) {
		if (i != survivor_index) {
			//population[i].network.killNetwork();
			population[i].network.copyNetwork(
				population[survivor_index].network);
			printf("copiou a rede\n");
			population[i].network.mutate(rand() + i, 0.05,
										 0.2);
		}
		int temp_x, temp_y;
		do {
			temp_x = rand() % gridSize;
			temp_y = rand() % gridSize;
		} while (grid[temp_x][temp_y] == OCCUPIED);
		population[i].line = temp_x;
		population[i].column = temp_y;
		grid[temp_x][temp_y] = OCCUPIED;
		population[i].alive = true;
		population[i].n_dodges = 0;
		population[i].survival_time = 0;
	}
	printf("saiundo do genocidio\n");
}

void elitistBreed()
{
	//printf("entrando no elitist breed\n");
	/*if(population[pop_outputs.at(0).first].alive){
		printf("cometendo genocidio\n");
		genocide(pop_outputs.at(0).first);
		alive_pop = pop_size;
		return;
	}*/
	agent *new_population = (agent *)malloc(pop_size * sizeof(agent));

	clearGrid();

	float mutation_chance, mutation_range;

	int counter = 0;
	for (int i = 0; i < (pop_size - alive_pop); i++) {
		int temp_x;
		int temp_y;
		do {
			temp_x = rand() % gridSize;
			temp_y = rand() % gridSize;
		} while (grid[temp_x][temp_y] == OCCUPIED);

		new_population[counter].line = temp_x;
		new_population[counter].column = temp_y;
		grid[temp_x][temp_y] = OCCUPIED;

		if (population[pop_outputs.at(i).first].n_dodges > 0) {
			mutation_chance =
				(MAX_MUTATION_CHANCE - MIN_MUTATION_CHANCE) *
					(1 -
					 ((float)(population[pop_outputs.at(0).first].survival_time +
							  population[pop_outputs.at(i).first].survival_time) /
					  (float)(2 * generation_duration))) +
				MIN_MUTATION_RANGE;

			mutation_range =
				(MAX_MUTATION_RANGE - MIN_MUTATION_RANGE) *
					((float)(population[pop_outputs.at(0).first].survival_time +
							 population[pop_outputs.at(i).first].survival_time) /
					 (float)(2 * generation_duration)) +
				MIN_MUTATION_RANGE;
			//printf("melhor:\n");
			//population[pop_outputs.at(0).first].network.printLastLayer();
			//printf("outro pai:\n");
			//population[pop_outputs.at(i).first].network.printLastLayer();
			new_population[counter].network =
				reproduce(population[pop_outputs.at(0).first].network,
						  population[pop_outputs.at(i).first].network, LAYERS,
						  mode, true, rand(), mutation_range, mutation_chance);
			//printf("filho:\n");
			//new_population[counter].network.printLastLayer();
		} else {
			new_population[counter].network =
				Network(n_layers, n_inputs, n_per_l, true, rand());
		}

		new_population[counter].alive = true;
		new_population[counter].survival_time = 0;
		new_population[counter].prev_pos = 0;
		new_population[counter].n_dodges = 0;

		counter++;
	}

	// Adding the alive agents from the last generation to the new one
	int idx = 0;

	for (int i = counter; i < pop_size; i++) {
		int temp_x;
		int temp_y;
		do {
			temp_x = rand() % gridSize;
			temp_y = rand() % gridSize;
		} while (grid[temp_x][temp_y] == OCCUPIED);

		new_population[counter].line = temp_x;
		new_population[counter].column = temp_y;
		grid[temp_x][temp_y] = OCCUPIED;
		new_population[counter].network =
			population[pop_alive[idx].first].network;
		new_population[counter].alive = true;
		new_population[counter].survival_time = 0;
		new_population[counter].n_dodges = 0;

		counter++;
		idx++;
	}

	alive_pop = counter;

	// kill previous generation
	for (int i = 0; i < pop_size; i++) {
		population[i].network.killNetwork();
	}
	free(population);

	population = new_population;
	//printf("saindo do elitist breed\n");
}

void nBestBreed()
{
	//printf("Entrou em nBestBreed\n");
	agent *new_population = (agent *)malloc(pop_size * sizeof(agent));

	clearGrid();

	float mutation_chance, mutation_range;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, n_best - 1);

	int counter = 0;
	//printf("n best: %d\n", n_best);
	for (int i = 0; i < (pop_size - alive_pop); i++) {
		//printf("counter: %d\n", counter);
		// Add the result to the new list
		int temp_x;
		int temp_y;
		do {
			temp_x = rand() % gridSize;
			temp_y = rand() % gridSize;
		} while (grid[temp_x][temp_y] == OCCUPIED);

		int father = dis(gen);
		int mother = dis(gen);

		if (father == mother) {
			mother = dis(gen);
		}

		mutation_chance =
			(MAX_MUTATION_CHANCE - MIN_MUTATION_CHANCE) *
				(1 - ((float)(population[pop_outputs.at(father).first]
								  .survival_time +
							  population[pop_outputs.at(mother).first]
								  .survival_time) /
					  (float)(2 * generation_duration))) +
			MIN_MUTATION_RANGE;

		mutation_range =
			(MAX_MUTATION_RANGE - MIN_MUTATION_RANGE) *
				((float)(population[pop_outputs.at(father).first].survival_time +
						 population[pop_outputs.at(mother).first].survival_time) /
				 (float)(2 * generation_duration)) +
			MIN_MUTATION_RANGE;

		new_population[counter].line = temp_x;
		new_population[counter].column = temp_y;
		grid[temp_x][temp_y] = OCCUPIED;
		new_population[counter].network =
			reproduce(population[pop_outputs.at(father).first].network,
					  population[pop_outputs.at(mother).first].network, LAYERS,
					  mode, true, rand(), mutation_range, mutation_chance);
		new_population[counter].alive = true;
		new_population[counter].survival_time = 0;
		new_population[counter].n_dodges = 0;
		//printf("gerando individuo %d\n", counter);
		counter++;
	}

	// Adding the alive agents from the last generation to the new one
	int idx = 0;

	for (int i = counter; i < pop_size; i++) {
		int temp_x;
		int temp_y;
		do {
			temp_x = rand() % gridSize;
			temp_y = rand() % gridSize;
		} while (grid[temp_x][temp_y] == OCCUPIED);

		new_population[counter].line = temp_x;
		new_population[counter].column = temp_y;
		grid[temp_x][temp_y] = OCCUPIED;
		new_population[counter].network =
			population[pop_alive[idx].first].network;
		new_population[counter].alive = true;
		new_population[counter].survival_time = 0;
		new_population[counter].n_dodges = 0;

		counter++;
		idx++;
	}

	alive_pop = counter;

	// kill previous generation
	for (int i = 0; i < pop_size; i++) {
		//printf("killing network %d of %d\n", i, pop_size);
		population[i].network.killNetwork();
	}

	//printf("dando free\n");
	free(population);

	population = new_population;
}

void assexualReproduction()
{
	clearGrid();

	float mutation_chance = (MAX_MUTATION_CHANCE - MIN_MUTATION_CHANCE) *
								((float)alive_pop / (float)pop_size) +
							MIN_MUTATION_CHANCE,
		  mutation_range;

	int counter = 0;
	for (int i = 0; i < pop_size; i++) {
		//printf("counter: %d\n", counter);
		// Add the result to the new list
		int temp_x;
		int temp_y;
		do {
			temp_x = rand() % gridSize;
			temp_y = rand() % gridSize;
		} while (grid[temp_x][temp_y] == OCCUPIED);

		mutation_range =
			(MAX_MUTATION_RANGE - MIN_MUTATION_RANGE) *
				((float)population[pop_outputs.at(counter).first].survival_time /
				 (float)generation_duration) +
			MIN_MUTATION_RANGE;

		population[counter].line = temp_x;
		population[counter].column = temp_y;
		grid[temp_x][temp_y] = OCCUPIED;
		population[counter].network.mutate(rand(), mutation_range,
										   mutation_chance);
		population[counter].alive = true;
		population[counter].survival_time = 0;
		population[counter].n_dodges = 0;
		//printf("gerando individuo %d\n", counter);
		counter++;
	}
	alive_pop = counter;
}

void updateGrid(bean a_bean)
{
	if (a_bean.type) {
		for (int i = 0; i < gridSize; i++) {
			grid[a_bean.coordinate][i] = a_bean.counter;
		}
	} else {
		for (int i = 0; i < gridSize; i++) {
			grid[i][a_bean.coordinate] = a_bean.counter;
		}
	}
}

void updateGrid(agent an_agent)
{
	grid[an_agent.line][an_agent.column] = OCCUPIED;
}

void clearGrid()
{
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			grid[i][j] = 0;
		}
	}
}

void clearGrid(bool keep_agents)
{
	if (keep_agents) {
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				if (grid[i][j] != OCCUPIED) {
					grid[i][j] = 0;
				}
			}
		}
	}
}

void removeFromGrid(agent an_agent)
{
	int values[4];
	if (an_agent.column == gridSize - 1) {
		values[0] = OCCUPIED;
	} else {
		values[0] = grid[an_agent.line][an_agent.column + 1];
	}
	if (an_agent.column == 0) {
		values[1] = OCCUPIED;
	} else {
		values[1] = grid[an_agent.line][an_agent.column - 1];
	}
	if (an_agent.line == gridSize - 1) {
		values[2] = OCCUPIED;
	} else {
		values[2] = grid[an_agent.line + 1][an_agent.column];
	}
	if (an_agent.line == 0) {
		values[3] = OCCUPIED;
	} else {
		values[3] = grid[an_agent.line - 1][an_agent.column];
	}

	grid[an_agent.line][an_agent.column] =
		std::max_element(std::begin(values), std::end(values))[0];
}

void updatePopulation()
{
	for (int i = 0; i < pop_size; i++) {
		if (population[i].alive &&
			grid[population[i].line][population[i].column] >= bean_delay) {
			population[i].alive = false;
			removeFromGrid(population[i]);
			alive_pop--;
			//printf("individuo %d morreu, alive_pop = %d\n", i, alive_pop);
		}
	}
}

void drawBean(bean a_bean)
{
	float x1, x2, y1, y2;
	if (a_bean.type) {
		//printf("bean de linha\n");
		x1 = -1;
		x2 = 1;
		y1 = -1 + 2 * (float)a_bean.coordinate / (float)gridSize;
		y2 = y1 + (2 / (float)gridSize);
	} else {
		//printf("bean de coluna\n");
		x1 = -1 + 2 * (float)a_bean.coordinate / (float)gridSize;
		x2 = x1 + 2 / (float)gridSize;
		y1 = -1;
		y2 = 1;
	}
	glColor3f((float)a_bean.counter / bean_delay,
			  (float)a_bean.counter / bean_delay,
			  (float)a_bean.counter / bean_delay);
	//glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex2d(x1, y1);
	glVertex2d(x1, y2);
	glVertex2d(x2, y2);
	glVertex2d(x2, y1);
	glEnd();
}

float padding = 0; //0.4;

void drawAgent(agent an_agent, int i)
{
	/*TODO:
        1 -fix padding
    */
	float padding_absolute = padding * (2 / (float)gridSize),
		  x1 = -1 + 2 * (float)an_agent.column / (float)gridSize +
			   padding_absolute,
		  x2 = x1 + (2 / (float)gridSize) - padding_absolute,
		  y1 = -1 + 2 * (float)an_agent.line / (float)gridSize +
			   padding_absolute,
		  y2 = y1 + (2 / (float)gridSize) - padding_absolute;
	if (an_agent.alive) {
		glColor3f(0, 0, 0.8);
	} else {
		glColor3f(0.6, 0, 0);
	}
	glBegin(GL_POLYGON);
	glVertex2d(x1, y1);
	glVertex2d(x1, y2);
	glVertex2d(x2, y2);
	glVertex2d(x2, y1);
	glEnd();

	glColor3f(1, 0, 1);
	float x = x1 + ((float)gridSize), y = y1 + ((float)gridSize);
	glRasterPos2f(x, y);
	char c = '0' + i;
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

bool print_once = true;

void print_csv()
{
	FILE *csv = fopen("csv.out", "w");

	fprintf(csv, "generation,score,time,n dodges\n");

	for (int i = 0; i < max_generations; i++) {
		fprintf(csv, "%d,%f,%d,%d\n", i, best_score_of_n[i], best_time_of_n[i],
				best_dodges_of_n[i]);
	}

	fclose(csv);

	print_once = false;
}

void draw()
{
	//printf("no draw\n");
	glClear(GL_COLOR_BUFFER_BIT);
	if (running) {
		for (int i = 0; i < pop_size; i++) {
			//if(population[i].alive){
			drawAgent(population[i], i);
			//}
		}
		for (int i = 0; i < n_beans; i++) {
			drawBean(beans[i]);
		}
		glColor3f(0.6, 0.6, 1);
		float x = -0.9, y = 0.9;
		glRasterPos2f(x, y);
		std::string string = "generation: " + std::to_string(n_generations) +
							 " alive: " + std::to_string(alive_pop);
		for (char c : string) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	} else {
		int min, max;
		if (max_generations <= 20) {
			min = 0;
			max = max_generations;
		} else {
			min = max_generations - 20;
			max = max_generations;
		}
		glColor3f(0.6, 0.6, 1);
		float x = -0.9, y = 0.9;
		for (int i = min; i < max; i++) {
			glRasterPos2f(x, y);
			std::string end_message =
				"in generation " + std::to_string(i) + " best is " +
				std::to_string(best_score_of_n[i]) + " and " +
				std::to_string(alive_to_the_end[i]) + " survived to the end";
			for (char c : end_message) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
			}
			y -= 0.08;
		}

		if (print_once) {
			print_csv();
			std::cout << "Use \"make plot\" to plot the simulation data\n";
		}

		//sleep(10000);
	}
	glutSwapBuffers();
}

void printGrid()
{
	//printf("printando grid\n");
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			printf("%d ", grid[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void getGridData(float *destiny, int i_agent, int j_agent)
{
	destiny[0] = 1;
	/*int counter = 1;
  	for(int i = -1; i <= 1; i++){
      	for(int j = -1; j <=1; j++){
          	if((i_agent+i < 0 || i_agent+i >= gridSize) || (j_agent+j < 0 || j_agent+j >= gridSize))
              	destiny[counter] = -1;
          	else
              	destiny[counter] = grid[i_agent+i][j_agent+j];
          	counter++;
        }
    }
  	destiny[counter] = bean_delay;*/
	if (i_agent == gridSize - 1) {
		destiny[1] = OCCUPIED;
	} else {
		destiny[1] = grid[i_agent + 1][j_agent];
	}
	//----------------------
	if (i_agent == 0) {
		destiny[2] = OCCUPIED;
	} else {
		destiny[2] = grid[i_agent - 1][j_agent];
	}
	//----------------------
	if (j_agent == gridSize - 1) {
		destiny[3] = OCCUPIED;
	} else {
		destiny[3] = grid[i_agent][j_agent + 1];
	}
	//-----------------------
	if (j_agent == 0) {
		destiny[4] = OCCUPIED;
	} else {
		destiny[4] = grid[i_agent][j_agent - 1];
	}
	destiny[5] = grid[i_agent][j_agent];
	//printf("data do quadrado: %f\n", destiny[5]);
}

void movePlayer(int i)
{
	int decision, new_line = population[i].line,
				  new_column = population[i].column;
	float environment_data[6];
	getGridData(environment_data, population[i].line, population[i].column);
	decision = population[i].network.runSoftmax(environment_data);
	//printf("going %s\n", strings[decision]);
	switch (decision) {
	case UP:
		new_line = population[i].line + 1;
		if (new_line >= gridSize) {
			new_line = population[i].line;
		}
		new_column = population[i].column;
		break;
	case DOWN:
		new_line = population[i].line - 1;
		if (new_line < 0) {
			new_line = population[i].line;
		}
		new_column = population[i].column;
		break;
	case RIGHT:
		new_column = population[i].column + 1;
		if (new_column >= gridSize) {
			new_column = population[i].column;
		}
		new_line = population[i].line;
		break;
	case LEFT:
		new_column = population[i].column - 1;
		if (new_column < 0) {
			new_column = population[i].column;
		}
		new_line = population[i].line;
		break;
	}
	if (grid[new_line][new_column] != OCCUPIED) {
		population[i].prev_pos = grid[population[i].line][population[i].column];
		removeFromGrid(population[i]);
		population[i].line = new_line;
		population[i].column = new_column;
		population[i].new_pos = grid[new_line][new_column];
		updateGrid(population[i]);
		if (population[i].prev_pos > population[i].new_pos) {
			population[i].n_dodges++;
		}
		//printf("nova posicao [%d, %d] registrada no grid\n", population[i].line, population[i].column);
	}

	population[i].survival_time++;
}

void breed()
{
	switch (breeding_type) {
	case ELITIST:
		elitistBreed();
		break;
	case N_BEST:
		nBestBreed();
		break;
	case ASSEXUAL:
		assexualReproduction();
		break;
	default:
		std::cout
			<< "Escolha um tipo válido de cruzamento: ELITIST = 0, N_BEST = 1, ASSEXUAL = 2\n";
		std::cin >> breeding_type;
		breed();
	}
}

void simulation(int)
{
	if (running) {
		clearGrid();
		if (n_beans < max_beans) {
			beans[n_beans] = { bool(rand() % 2), rand() % gridSize, 0 };
			n_beans++;
		}
		for (int i = 0; i < n_beans; i++) {
			if (beans[i].counter < bean_delay) {
				beans[i].counter++;
				updateGrid(beans[i]);
				//std::cout<<"incrementando counter de "<<i<<" : "<<beans[i].counter<<" "<<beans[i].coordinate<<"\n";
			} else {
				beans[i] = { bool(rand() % 2), rand() % gridSize, 0 };
			}
		}
		updatePopulation();
		for (int i = 0; i < pop_size; i++) {
			if (population[i].alive) {
				movePlayer(i);
			}
		}

		//printf("n generation: %d generation counter = %d pop_alive = %d\n", n_generations,generation_counter, alive_pop);
		//printGrid();

		if (generation_counter >= generation_duration || alive_pop == 0) {
			//printf("Entrou no if generation\n");
			n_generations++;
			bool use_time = false;
			if (alive_pop > 0) {
				use_time = true;
			}
			checkBest(use_time);

			if (n_generations > max_generations) {
				running = false;
			} else {
				breed();
				generation_counter = 0;
				n_beans = 0;
			}
		}

		generation_counter++;
	}

	glutPostRedisplay();
	glutTimerFunc(4000 / 60, simulation, 0);
}
