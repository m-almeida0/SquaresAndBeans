# The Simulation
<!-- Explicação geral de como o "jogo" funciona 
(quais são os possíveis movimentos que o agente pode fazer, 
como os raios funcionam, o quê o agente consegue ver etc) -->

## How the Reproduction Works
<!-- explicação do funcionamento dos diferentes tipos de reprodução -->

## "Splicing" (Or Crossing Over)
<!-- explicação dos diferentes tipos de "splicing" -->

## The Fitness Evluation Function
<!-- explicação da função fitness 
(como é decido que indivíduo é melhor que o outro, 
explicando como é calculado o score etc) -->

## The Neural Network
<!-- Representação gráfica da Rede Neural -->

# Running the Simulation

## Suggested Values to Test

## Plotting the Results

Use the `make plot` command to run the `plot_csv.py` script, or run it directly
by typing the `python3 plot_csv.py` command. If you decide to run the script 
directly and you are on a Wayland system, you have to remember to set the 
`QT_QPA_PLATFORM=wayland` environment variable beforehand.

# Building the Application

The building process is pretty straight forward. First you'll need to clone the repository with:

```
git clone https://github.com/m-almeida0/SquaresAndBeans.git
```
```
cd SquaresAndBeans/
```

Next you'll need to download some dependencies. In the following sections we have the commands you can use on two popular GNU/Linux distributions (if you're using Windows or MacOS, well... good luck :D. You can use WSL on Windows if you want though).

## Fedora
```
sudo dnf install make g++ freeglut-devel python3-matplotlib-qt5
```

## Ubuntu

```
sudo apt install make g++ freeglut3-dev python3-matplotlib python3-pyqt5
```

After having installed the needed dependencies, you can run `make` to build the application. In the next section, each possible `make` command is listed and explained.

## Compiling
<!-- como compilar (o quê cada comando do make faz) -->
- `make clean`: deletes all previous compilation artifacts

- `make game`: generate `squares.o` and `NNetworks.o` object files if it does 
not exists, or if it is outdated, and then compiles the `game` binary if it
also does not exists or is outdated

- `make`/`make all`: does `clean` and then `game`

- `make run`: does `game` and then runs the simulation

