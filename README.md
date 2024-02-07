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
by typing the `python plot_csv.py` command. If you decide to run the script 
directly and you are on a Wayland system, you have to remember to set the 
`QT_QPA_PLATFORM=wayland` environment variable beforehand.

** See the `Python Dependencies` section.

# Building the Application

<!-- quais dependências são necessárias para compilar o código (openGL etc). 
Também seria legal colocar os comandos para instalar em distros baseadas em 
Ubuntu e RHEL/Fedora (já que são as mais famosas geralmente) -->

## C++ Dependencies

## Python Dependencies

- matplotlib

- PyQt5

## Compiling
<!-- como compilar (o quê cada comando do make faz) -->
- `make clean`: deletes all previous compilation artifacts

- `make game`: generate `squares.o` and `NNetworks.o` object files if it does 
not exists, or if it is outdated, and then compiles the `game` binary if it
also does not exists or is outdated

- `make`/`make all`: does `clean` and then `game`

- `make run`: does `game` and then runs the simulation

