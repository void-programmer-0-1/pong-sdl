compiler=g++
libs=-lSDL2 -lSDL2_ttf -lSDL2_mixer

all:main.o Game.o
	${compiler} main.o Game.o ${libs} -o game
	rm main.o
	rm Game.o

main.o:main.cpp
	${compiler} ${libs} -c main.cpp

Game.o:Game.cpp
	${compiler} ${libs} -c Game.cpp

play:game
	./game

clear:game
	rm game