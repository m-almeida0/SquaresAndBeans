FLAGS = -march=native -O2 -Wall -Wextra -Wunused-result

all: clean game

game: NNetworks.o
	g++ $(FLAGS) squares.cpp NNetworks.o -o squares -lglut -lGLU -lGL

NNetworks.o:
	g++ $(FLAGS) -c NNetworks.cpp -o NNetworks.o

clean:
	rm -f *.o
	rm -f squares
	clear

run:
	./squares
