#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif
#include"NNetworks.h"
#include <algorithm>
#include <vector>
#include <utility>
#include <cstdio>
#include<iostream>
#include <sstream>
#include <unistd.h>

#define windowWidth 800
#define windowHeight 800

#define STAY 0
#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4
char strings[5][7] = {"stay\0", "up\0", "down\0", "right\0", "left\0"};

int** grid;

//TODO: dar cores aos agentes e colocar os filhos para herdarem as cores dos pais
typedef struct _agent{
    int line;
    int column;
    Network network;
  	int survival_time;
  	bool alive;
	int prev_pos;
	int new_pos;
	int n_dodges;
}agent;
agent*population;
int alive_pop;

typedef struct _bean{
    bool type;//linha ou coluna
    int coordinate;
    double counter;
}bean;
bean*beans;

double boundedRand(int seed, double min, double max){
    //(static_cast<double>(std::rand())/RAND_MAX)
    return min+(static_cast<double>(std::rand())/RAND_MAX)*(max-min);
}

void draw();
void simulation(int);
int gridSize;
int pop_size;
double bean_delay;
int max_beans;
int n_beans = 0;
unsigned int bean_index = 0;
int generation_counter = 0;
int generation_duration;
int n_generations = 0;
int n_best = 0; // number of elements to use on breedNBest
std::vector<std::pair<int, double>> pop_outputs;
agent bestOfAll;

int mode;
int breeding;

void clearGrid();

int window;
int max_generations;
double *best_of_n;
int *alive_to_the_end;
bool running;
int n_inputs = 11, n_layers = 2, n_per_l[2] = {5, 5};

int slow_down = 10;
void keyboard(unsigned char key, int x, int y);
int main(int argc, char*argv[]){
    glutInit(&argc, argv);
    std::srand(getCurrentTimeInSeconds());

	bestOfAll.survival_time = -1;

	std::cout<<"insira o tamanho do grid ";
	std::cin>>gridSize;
	std::cout<<"insira o número de indivíduos, a duração de uma geração em frames e o numero de geracoes (nessa ordem, separado por epaços)\n";
	std::cin>>pop_size>>generation_duration>>max_generations;
	std::cout<<"insira o número de raios que você quer atingindo a população, e o número de frames até um deles matar\n";
	std::cin>>max_beans>>bean_delay;
	std::cout<<"insira o modo: AVERAGE = 0, SPLICING_HALF = 1, SPLICING_RAND = 2\n";
	std::cin>>mode;
	//std::cout<<"por fim, insira o modo de cruzamento: N Best = 0, Walrus = 1\n";
	//std::cin>>breeding;

    grid = (int**) malloc(gridSize*sizeof(int*));
    for(int i = 0; i < gridSize; i++){
        grid[i] = (int*) malloc(gridSize*sizeof(int));
        for(int j = 0; j < gridSize; j++)
            grid[i][j] = 0;
    }
    alive_pop = pop_size; n_best = pop_size/4;
	population = (agent*) malloc(pop_size*sizeof(agent));
	best_of_n = (double*) malloc(max_generations*sizeof(double));
	alive_to_the_end = (int*) malloc(max_generations*sizeof(int));
  	int temp_x, temp_y;
  	for(int i = 0; i < pop_size; i++){
      	do{
        	temp_x = rand()%gridSize; temp_y = rand()%gridSize;
      	}while(grid[temp_x][temp_y] == -1);
      	population[i].line = temp_x;
      	population[i].column = temp_y;
      	grid[temp_x][temp_y] = -1;
      	population[i].network = Network(n_layers, n_inputs, n_per_l, true, getCurrentTimeInSeconds());
        population[i].alive = true;
		population[i].survival_time = 0;
		population[i].prev_pos = 0;
		population[i].n_dodges = 0;
    }
    beans = (bean*) malloc(max_beans*sizeof(bean));

	running = true;
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(120, 120);
    window = glutCreateWindow("sumulacao");
    glClearColor(0, 0, 0, 1.0);
    glutDisplayFunc(draw);
    glutTimerFunc(0, simulation, 0);
	glutKeyboardFunc(keyboard);
    glutMainLoop();

	printf("saiu do game\n");
    free(beans);
	printf("deu free no beans\n");
    for(int i = 0; i < pop_size; i++){
        population[i].network.killNetwork();
    }
  	free(population);
	printf("deu free na populacao\n");
  	for(int i = 0; i < gridSize; i++){
        free(grid[i]);
    }
  	free(grid);
	printf("deu free no grid\n");
	free(best_of_n);
	free(alive_to_the_end);
    return 0;
}

void keyboard(unsigned char key, int x, int y){
	switch (key)
	{
	case 's':
		if(slow_down > 10)
			slow_down = 10;
		else
			slow_down++;
		break;
	
	case 'f':
		if(slow_down < 1)
			slow_down = 1;
		else
			slow_down++;
	}
}

void checkBest() {
	pop_outputs.clear();	

  	for (int i = 0; i < pop_size; i++) {
      	//checar se deu merda, talvez seja necessŕio usar "at"
		pop_outputs.push_back(std::make_pair(i, (double) (population[i].survival_time * population[i].n_dodges) / (double) generation_duration));
		//printf("Conferindo validade do network %d: %d\n", i, population[i].network.NNeuronsInLayerN(0));
    }
  
  	std::sort(pop_outputs.begin(), pop_outputs.end(), [](const auto& lhs, const auto& rhs) {return lhs.second > rhs.second;});

	printf("best of this generation: survival time = %d, n_dodges = %d, pontuation = %lf \n", population[pop_outputs.at(0).first].survival_time, population[pop_outputs.at(0).first].n_dodges, pop_outputs.at(0).second);
	best_of_n[n_generations-1] = pop_outputs.at(0).second;
	alive_to_the_end[n_generations-1] = alive_pop;

	//printf("best of all: survival time: %d, n dodges: %d\n", bestOfAll.survival_time);
  	
	if (((double) (bestOfAll.survival_time * bestOfAll.n_dodges) / (double) generation_duration) < pop_outputs.at(0).second) {
		//printf("Entrou best of all\n");
		bestOfAll = population[pop_outputs.at(0).first];
		//printf("popoutputs: %d\n", pop_outputs.at(0).first);
		//printf("sla: %d\n", population[pop_outputs.at(0).first].network.NNeuronsInLayerN(0));
		//bestOfAll.network.copyNetwork(population[pop_outputs.at(0).first].network);
		bestOfAll.network = population[pop_outputs.at(0).first].network;
		//printf("saiu best of all\n");
	}
}

void nBestBreed() {
	//printf("Entrou em nBestBreed\n");
  	agent *new_population = (agent*) malloc(pop_size*sizeof(agent));

	clearGrid();

	double mutation_chance = 0.45 * ((double) alive_pop / (double) pop_size) + 0.05, mutation_range;

	int counter = 0;
	//printf("n best: %d\n", n_best);
    for (int i = 0; i < pop_size; i++) {
		//printf("counter: %d\n", counter);
        // Add the result to the new list
		int temp_x; int temp_y;
		do{
			temp_x = rand()%gridSize; temp_y = rand()%gridSize;
		}while(grid[temp_x][temp_y] == -1);

		int father = rand() % n_best;
		int mother = rand() % n_best;

		if (father == mother) {
			int mother = (mother == n_best - 1) ? mother - 1 : mother + 1;
		}

		mutation_range = 0.5 * ((double) (pop_outputs.at(0).second+pop_outputs.at(i).second) / (double) (2*generation_duration)) + 0.1;

		new_population[counter].line = temp_x;
		new_population[counter].column = temp_y;
		grid[temp_x][temp_y] = -1;
        new_population[counter].network = reproduce(population[pop_outputs.at(father).first].network, population[pop_outputs.at(mother).first].network, NEURONS, mode, true, (int) getCurrentTimeInSeconds(), mutation_range, mutation_chance, NULL);
		new_population[counter].alive = true;
		new_population[counter].survival_time = 0;
		new_population[counter].n_dodges = 0;
		//printf("gerando individuo %d\n", counter);
		counter++;
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


void walrusBreed() {
	agent *new_population = (agent*) malloc(pop_size*sizeof(agent));

	clearGrid();

	double mutation_chance = 0.45 * ((double) alive_pop / (double) pop_size) + 0.05, mutation_range;

	int counter = 0;
	for (int i = 0; i < pop_size; i++) {
		if (population[pop_outputs.at(i).first].n_dodges > 0) {
			int temp_x; int temp_y;
			do{
				temp_x = rand()%gridSize; temp_y = rand()%gridSize;
			}while(grid[temp_x][temp_y] == -1);
			mutation_range = 0.5 * ((double) (pop_outputs.at(0).second+pop_outputs.at(i).second) / (double) (2*generation_duration)) + 0.1;
			new_population[counter].line = temp_x;
			new_population[counter].column = temp_y;
			grid[temp_x][temp_y] = -1;
			new_population[counter].network = reproduce(population[pop_outputs.at(0).first].network, population[pop_outputs.at(i).first].network, NEURONS, mode, true, (int) getCurrentTimeInSeconds(), mutation_range, mutation_chance, NULL);
			new_population[counter].alive = true;
			new_population[counter].survival_time = 0;
			new_population[counter].prev_pos = 0;
			new_population[counter].n_dodges = 0;
		} else {
			int temp_x; int temp_y;
			do{
				temp_x = rand()%gridSize; temp_y = rand()%gridSize;
			}while(grid[temp_x][temp_y] == -1);
			new_population[counter].line = temp_x;
			new_population[counter].column = temp_y;
			grid[temp_x][temp_y] = -1;
			new_population[counter].network = Network(n_layers, n_inputs, n_per_l, true, getCurrentTimeInSeconds());
			new_population[counter].alive = true;
			new_population[counter].survival_time = 0;
			new_population[counter].prev_pos = 0;
			new_population[counter].n_dodges = 0;
		}
		counter++;
	}

	alive_pop = counter;

	// kill previous generation
	for (int i = 0; i < pop_size; i++) {
		population[i].network.killNetwork();
	}
	free(population);

	population = new_population;
}

void updateGrid(bean a_bean) {
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

void updateGrid(agent an_agent) {
    grid[an_agent.line][an_agent.column] = -1;
}

void clearGrid() {
    for (int i = 0; i < gridSize; i++) {
    	for (int j = 0; j < gridSize; j++) {
    		grid[i][j] = 0;
        }
	}
}

void clearGrid(bool keep_agents) {
  	if(keep_agents){
    	for (int i = 0; i < gridSize; i++) {
    		for (int j = 0; j < gridSize; j++) {
    			if(grid[i][j] != -1)
	              	grid[i][j] = 0;
        	}
		}
  	}
}

void removeFromGrid(agent an_agent){
  	int values[4];
  	if(an_agent.column == gridSize-1)
      	values[0] = -1;
  	else
      	values[0] = grid[an_agent.line][an_agent.column+1];
  	if(an_agent.column == 0)
      	values[1] = -1;
  	else
      	values[1] = grid[an_agent.line][an_agent.column-1];
  	if(an_agent.line == gridSize-1)
      	values[2] = -1;
  	else
      	values[2] = grid[an_agent.line+1][an_agent.column];
  	if(an_agent.line == 0)
      	values[3] = -1;
  	else
      	values[3] = grid[an_agent.line-1][an_agent.column];
  	
  	grid[an_agent.line][an_agent.column] = std::max_element(std::begin(values), std::end(values))[0];
}

void updatePopulation(){
  for(int i = 0; i < pop_size; i++){
    	if(population[i].alive && grid[population[i].line][population[i].column] >= bean_delay){
          	population[i].alive = false;
         	removeFromGrid(population[i]);
            alive_pop --;
            //printf("individuo %d morreu, alive_pop = %d\n", i, alive_pop);
        }
  }
}

void drawBean(bean a_bean){
    double x1, x2, y1, y2;
    if(a_bean.type){
        //printf("bean de linha\n");
        x1 = -1; x2 = 1; y1 = -1+2*(double)a_bean.coordinate/(double) gridSize; y2 = y1+(2/(double) gridSize);
    }else{
        //printf("bean de coluna\n");
        x1 = -1+2*(double)a_bean.coordinate/(double) gridSize; x2 = x1+2/(double) gridSize; y1 = -1; y2 = 1;
    }
    glColor3f((double)a_bean.counter/bean_delay, (double)a_bean.counter/bean_delay, (double)a_bean.counter/bean_delay);
    //glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex2d(x1, y1);
    glVertex2d(x1, y2);
    glVertex2d(x2, y2);
    glVertex2d(x2, y1);
    glEnd();
}

double padding = 0;//0.4;
void drawAgent(agent an_agent, int i){
    /*TODO:
        1 -fix padding
    */
  	double padding_absolute = padding*(2/(double) gridSize),
    x1 = -1+2*(double)an_agent.column/(double) gridSize+padding_absolute,
  	x2 = x1 + (2/(double) gridSize)-padding_absolute,
  	y1 = -1+2*(double)an_agent.line/(double) gridSize+padding_absolute,
  	y2 = y1+(2/(double) gridSize)-padding_absolute;
	if(an_agent.alive)
  		glColor3f(0, 0, 0.8);
	else
		glColor3f(0.6, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2d(x1, y1);
    glVertex2d(x1, y2);
    glVertex2d(x2, y2);
    glVertex2d(x2, y1);
    glEnd();

	glColor3f(1, 0, 1);
	double x = x1+((double) gridSize), y = y1+((double) gridSize);
	glRasterPos2f(x, y);
	char c = '0'+i;
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void draw(){
    //printf("no draw\n");
	glClear(GL_COLOR_BUFFER_BIT);
	if(running){
		for(int i = 0; i < pop_size; i++){
			//if(population[i].alive){
			drawAgent(population[i], i);
			//}
		}
		for(int i = 0; i < n_beans; i++)
			drawBean(beans[i]);
		glColor3f(0.6, 0.6, 1);
		double x = -0.9, y = 0.9;
		glRasterPos2f(x, y);
		std::string string = "generation: "+std::to_string(n_generations)+" alive: "+std::to_string(alive_pop);
		for (char c : string) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	}else{
		int min, max;
		if(max_generations <= 20){
			min = 0;
			max = max_generations;
		}else{
			min = max_generations - 20;
			max = max_generations;
		}
		glColor3f(0.6, 0.6, 1);
		double x = -0.9, y = 0.9;
		for(int i = min; i < max; i++){
			glRasterPos2f(x, y);
			std::string end_message = "in generation "+std::to_string(i)+" best is "+std::to_string(best_of_n[i])+" and "+std::to_string(alive_to_the_end[i])+" survived to the end";
			for (char c : end_message) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
			}
			y -=0.08;
		}
		for(int i = 0; i < max_generations; i++){
			std::cout<<i<<","<<best_of_n[i]<<"\n";
		}
		sleep(10000);
	}
	glutSwapBuffers();
}

void printGrid(){
    //printf("printando grid\n");
    for(int i = 0; i < gridSize; i++){
        for(int j = 0; j < gridSize; j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void getGridData(double*destiny, int i_agent, int j_agent){
 	destiny[0] = 1;
    int counter = 1;
  	for(int i = -1; i <= 1; i++){
      	for(int j = -1; j <=1; j++){
          	if((i_agent+i < 0 || i_agent+i >= gridSize) || (j_agent+j < 0 || j_agent+j >= gridSize))
              	destiny[counter] = -1;
          	else
              	destiny[counter] = grid[i_agent+i][j_agent+j];
          	counter++;
        }
    }
  	destiny[counter] = bean_delay;
}

void simulation(int){
	if(running){
		clearGrid();
		if(n_beans < max_beans){
			beans[n_beans] = {bool(rand()%2), rand()%gridSize, 0};
			n_beans++;
		}
		for(int i = 0; i < n_beans; i++){
			if(beans[i].counter < bean_delay){
				beans[i].counter++;
				updateGrid(beans[i]);
				//std::cout<<"incrementando counter de "<<i<<" : "<<beans[i].counter<<" "<<beans[i].coordinate<<"\n";
			}else{
				beans[i] = {bool(rand()%2), rand()%gridSize, 0};
			}
		}
		updatePopulation();
		int decision, new_line, new_column;
		double environment_data[11];
		for(int i = 0; i < pop_size; i++){
			if(population[i].alive){
				//printf("%d esta vivo. Em [%d, %d]\n", i, population[i].line, population[i].column);
				getGridData(environment_data, population[i].line, population[i].column);
				//printf("o agente %d esta %d e ve na sua posicao %d\n", i, population[i].alive, (int)environment_data[5]);
				//printf("passou do getGridData\n");
				decision = population[i].network.runSoftmax(environment_data, 0);
				//printf("\npassou da tomada de decisao, indo %d - %s\n",decision, strings[decision]);
				switch(decision){
					case UP:
						new_line = population[i].line+1;
						if(new_line >= gridSize) {
							new_line = population[i].line;
						}
						new_column = population[i].column;
						break;
					case DOWN:
						new_line = population[i].line-1;
						if(new_line < 0) {
							new_line = population[i].line;
						}
						new_column = population[i].column;
						break;
					case RIGHT:
						new_column = population[i].column+1;
						if(new_column >= gridSize) {
							new_column = population[i].column;
						}
						new_line = population[i].line;
						break;
					case LEFT:
						new_column = population[i].column-1;
						if(new_column < 0) {
							new_column = population[i].column;
						}
						new_line = population[i].line;
						break;
					default:
						new_line = population[i].line;
						new_column = population[i].column;
						//break;
				}
				//printf("passou do switch. novas coordenadas sao [%d, %d], com valor \n", new_line, new_column);
				if(grid[new_line][new_column] != -1){
					population[i].prev_pos = grid[population[i].line][population[i].column];
					//printf("entrou no if\n");
					removeFromGrid(population[i]);
					//printf("foi tirado do grid\n");
					population[i].line = new_line;
					population[i].column = new_column;
					population[i].new_pos = grid[new_line][new_column];
					updateGrid(population[i]);
					if(population[i].prev_pos > population[i].new_pos)
						population[i].n_dodges++;
					//printf("nova posicao [%d, %d] registrada no grid\n", population[i].line, population[i].column);
				}

				population[i].survival_time++;
			}
		}

		//printf("n generation: %d generation counter = %d pop_alive = %d\n", n_generations,generation_counter, alive_pop);
		//printGrid();

		if(generation_counter >= generation_duration || alive_pop == 0) {
			//printf("Entrou no if generation\n");
			n_generations++;
			checkBest();
			if(n_generations > max_generations)
				running = false;
			else{
				//nBestBreed();
				walrusBreed();
				generation_counter = 0;
				n_beans = 0;
			}
		}

		generation_counter++;
	}
	
    glutPostRedisplay();
    glutTimerFunc(slow_down*10/60, simulation, 0);
}
