# The Simulation <!-- Marcos -->
<!-- Explicação geral de como o "jogo" funciona 
(quais são os possíveis movimentos que o agente pode fazer, 
como os raios funcionam, o quê o agente consegue ver etc) -->

## How the Reproduction Works <!-- Miguel -->
<!-- explicação do funcionamento dos diferentes tipos de reprodução -->

## "Splicing" (Or Crossing Over) <!-- Marcos -->
<!-- explicação dos diferentes tipos de "splicing" -->

## The Fitness Evaluation Function <!-- Miguel -->
<!-- explicação da função fitness 
(como é decido que indivíduo é melhor que o outro, 
explicando como é calculado o score etc) -->

## The Neural Network <!-- Marcos -->
<!-- Representação gráfica da Rede Neural -->

# Running the Simulation <!-- Miguel -->

You can use `make run` or run the binary  directly with `./game` to run the 
simulation. After you launch it, the program will ask you for some parameters:

- **Tamanho do Grid:** defines how many rows and columns the game's internal
grid has. Each agent in the game occupies a single square inside the game's grid.

- **Tipo de Cruzamento:** defines the breeding type (what type of reproduction
will be used). It can be:

	- **"0" Elitist:** the best agent in the current generation breeds with
	every other agent;

	- **"1" N_Best:** the "N" best agents of the current generation breeds
	between themselves (the value of N is decided internally by the program based
	on how many agents are in a generation); or

	- **"2" Assexual:** there is no breeding, each agent creates a mutated copy 
	of themselves for the new generation.

- **Número de Indivíduos + Quantidade de Frames + Número de Gerações:** in this
part you will have to input 3 values separated by spaces: 

	- The amount of agents in each generation of the simulation (if you chose 
	the **N_Best** type of reproduction, the number of agents needs to be at 
	least 36);
	
	- The amount of frames until a generation ends (if all the agents die before
	the generation reaches this amount of frames the generation will end 
	anyway); and

	- How many generations you want to simulate.

- **Número de Raios + Quantos Frames até Matar:** in this part you will input 2
values separated by spaces:
	- The amount of beans you want being generated at the same time; and

	- The amount of frames until it actually kills an agent (if the agent is
	under it).

- **Modo:** this is the type of "splicing" (crossing-over), it can be:
	- **"0"** Average;

	- **"1"** Splicing_half; or

	- **"2"** Splicing_rand.

During the simulation you can use the "F" key in your keyboard to make the
simulation run faster (up to 8x faster), or use the "S" key to make the 
simulation run slower (down to 0.1x slower).

You can also use the "Q" key to quit the simulation, this will make so that the
progress made is saved to an csv file, this way you won't lose all the 
simulation data like you would if you just killed the program. 

**Be aware that if you quit the simulation, the data from  the generation you
were at the moment until the generation the simulation was supposed to run may
be filled with garbage and may not make sense. Only the data from the previous
generations are guaranteed to make sense.

## Suggested Values to Test <!-- Miguel -->

I suggest you test the following values:

- **100x100 grid:** 100;
- **N_BEST:** 1;
- **Agents:** 400, **Frames:** 100000, **Generations:** 100;
- **Beans:** 100, **Frames:** 10;
- **SPLICING_RAND:** 2.

## Plotting the Results

Use the `make plot` command to run the `plot_csv.py` script, or run it directly
by typing the `python3 plot_csv.py` command. If you decide to run the script 
directly and you are on a Wayland system, you have to remember to set the 
`QT_QPA_PLATFORM=wayland` environment variable beforehand.

**You need the `matplotlib` and `PyQt5` python packages installed to be able to 
use the plot functionality. 

# Building the Application

The building process is pretty straight forward. First you'll need to clone the 
repository with:

```
git clone https://github.com/m-almeida0/SquaresAndBeans.git
```
```
cd SquaresAndBeans/
```

Next you'll need to download some dependencies. In the following sections we 
have the commands you can use on two popular GNU/Linux distributions (if you're 
using Windows or MacOS, well... good luck :D. You can use WSL on Windows if you 
want though).

## Fedora
```
sudo dnf install make g++ freeglut-devel python3-matplotlib-qt5
```

## Ubuntu

```
sudo apt install make g++ freeglut3-dev python3-matplotlib python3-pyqt5
```

After having installed the needed dependencies, you can run `make` to build the 
application. In the next section, each possible `make` command is listed and 
explained.

## Compiling
<!-- como compilar (o quê cada comando do make faz) -->
- `make clean`: deletes all previous compilation artifacts;

- `make game`: generate `squares.o` and `NNetworks.o` object files if it does 
not exists, or if it is outdated, and then compiles the `game` binary if it
also does not exists or is outdated;

- `make`/`make all`: does `clean` and then `game`;

- `make run`: does `game` and then runs the simulation.

