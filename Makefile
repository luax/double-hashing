all: Main.out

Main.out: Main.cpp
	g++ -O3 -g *.cpp -Wall -std=c++0x -o Main.out
