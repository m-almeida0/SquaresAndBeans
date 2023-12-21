all: main

main: NNetworks.o
	g++ testMain.cpp NNetworks.o -o main

game: NNetworks.o
	g++ squares.cpp NNetworks.o -o squares -lglut -lGLU -lGL -Wall

NNetworks.o:
	g++ -c NNetworks.cpp -o NNetworks.o

clean:
	rm *.o
	clear

run:
	./main