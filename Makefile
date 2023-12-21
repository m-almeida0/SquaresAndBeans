all: game

game: NNetworks.o
	g++ squares.cpp NNetworks.o -o squares -lglut -lGLU -lGL -Wall

NNetworks.o:
	g++ -c NNetworks.cpp -o NNetworks.o

clean:
	rm *.o
	rm squares
	clear

run:
	./squares