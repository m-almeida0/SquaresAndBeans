#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include "NNetworks.h"
#include <cmath>

#define windowWidth 600
#define windowHeight 600
#define DEFAULT_LEN 0.06
#define gridSize 10

bool **grid;

class Bichinho {
private:
	double len;
	double calories;
	double attack;
	double defense;
	double speed;
	double consumedCalories;
	double x;
	double y;
	Network *decision;

public:
	Bichinho(double calories, double attack, double defense, double speed,
			 double x, double y);
	Bichinho();
	double getCalories();
	double getAttack();
	double getDefense();
	double getSpeed();
	double getLen();
	void setCoordinates(double x, double y);
	void add_calories(double eaten);
	double getx();
	double gety();
};

Bichinho::Bichinho()
{
	printf("I hope whoever invented OOP is burning in hell\n");
}

Bichinho::Bichinho(double calories, double attack, double defense, double speed,
				   double x, double y)
{
	this->calories = calories;
	this->attack = attack;
	this->defense = defense;
	this->speed = speed;
	this->len = DEFAULT_LEN;
	this->x = x;
	this->y = y;
	this->consumedCalories = 0;
}

double Bichinho::getCalories()
{
	return this->calories;
}

double Bichinho::getAttack()
{
	return this->attack;
}

double Bichinho::getDefense()
{
	return this->defense;
}

double Bichinho::getSpeed()
{
	return this->speed;
}

double Bichinho::getLen()
{
	return this->len;
}

void Bichinho::setCoordinates(double x, double y)
{
	this->x = x;
	this->y = y;
}

void Bichinho::add_calories(double eaten)
{
	this->consumedCalories += eaten;
}

double Bichinho::getx()
{
	return this->x;
}

double Bichinho::gety()
{
	return this->y;
}

double boundedRand(int seed, double min, double max)
{
	//(static_cast<double>(std::rand())/RAND_MAX)
	return min + (static_cast<double>(std::rand()) / RAND_MAX) * (max - min);
}

int population_size;
Bichinho *animais;
int food_amount;
int food_calories;
Bichinho *food;
int generation_counter = 0;
int generation_duration = 20;

void draw();
void simulation(int);

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	population_size = 12;
	int starting_calories = 2;
	animais = new Bichinho
		[population_size]; //(Bichinho*) malloc(population_size*sizeof(Bichinho));
	std::srand(getCurrentTimeInSeconds());
	for (int i = 0; i < population_size; i++) {
		animais[i] = Bichinho(starting_calories, 1.0 / 3, 1.0 / 3, 1.0 / 3,
							  boundedRand(rand(), -1, 1),
							  boundedRand(rand(), -1, 1));
	}
	food_amount = 20;
	food = (Bichinho *)malloc(food_amount * sizeof(Bichinho));
	for (int i = 0; i < food_amount; i++) {
		food[i] = Bichinho(food_calories, 0, 1, 0, boundedRand(rand(), -1, 1),
						   boundedRand(rand(), -1, 1));
	}
	food_calories = 2;
	grid = (bool **)malloc(gridSize * sizeof(bool *));
	for (int i = 0; i < gridSize; i++) {
		grid[i] = (bool *)malloc(gridSize * sizeof(bool));
		for (int j = 0; j < gridSize; j++) {
			grid[i][j] = false;
		}
	}

	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(120, 120);
	glutCreateWindow("nome da janela");
	glClearColor(0, 0, 0, 1.0);
	glutDisplayFunc(draw);
	glutTimerFunc(0, simulation, 0);
	glutMainLoop();
	return 0;
}

void draw_bichinho(Bichinho bichinho)
{
	double x = bichinho.getx(), y = bichinho.gety(), len = bichinho.getLen(),
		   calories = bichinho.getCalories();
	double red = bichinho.getAttack(), green = bichinho.getDefense(),
		   blue = bichinho.getSpeed();
	std::cout << "desenhando um bichinho em " << x << ", " << y
			  << " red: " << red << " green " << green << " blue " << blue
			  << "\n";
	glColor3f(red, green, blue);
	glBegin(GL_POLYGON);
	glVertex2d(x + len / 2, y + len / 2);
	glVertex2d(x + len / 2, y - len / 2);
	glVertex2d(x - len / 2, y - len / 2);
	glVertex2d(x - len / 2, y + len / 2);
	glEnd();
}

void draw()
{
	printf("no draw\n");
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < food_amount; i++) {
		printf("iteracao %d do loop\n", i);
		draw_bichinho(food[i]);
	}
	for (int i = 0; i < population_size; i++) {
		draw_bichinho(animais[i]);
	}
	glutSwapBuffers();
}

void simulation(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, simulation, 0);
}