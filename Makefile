FLAGS = -march=native -O2 -Wall -Wextra -Wunused-result

all: clean game

game: NNetworks.o squares.o
	g++ $(FLAGS) squares.o NNetworks.o -o game -lglut -lGLU -lGL

NNetworks.o: NNetworks.cpp NNetworks.h
	g++ $(FLAGS) -c NNetworks.cpp -o NNetworks.o

squares.o: squares.cpp
	g++ $(FLAGS) -c squares.cpp -o squares.o

run: game
	./game

clean:
	rm -f *.o
	rm -f squares
	clear

plot:
ifeq ($(XDG_SESSION_TYPE),wayland)
	QT_QPA_PLATFORM=wayland python3 plot_csv.py
else
	python plot_csv.py
endif

