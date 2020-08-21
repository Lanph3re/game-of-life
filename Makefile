all: main.o game-of-life.o life-runner.o
	g++ -o game-of-life -std=c++17 -lcurses \
		main.o \
		life-runner.o \
		game-of-life.o

main.o: include/game-of-life.h src/main.cc
	g++ -c src/main.cc -std=c++17

game-of-life.o: include/game-of-life.h src/game-of-life.cc
	g++ -c src/game-of-life.cc -std=c++17

life-runner.o: include/life-runner.h include/game-of-life.h src/life-runner.cc
	g++ -c src/life-runner.cc -std=c++17

clean:
	rm -f *.o game-of-life

