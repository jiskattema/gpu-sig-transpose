



all: naive

naive: naive.c
	gcc -march=native -O3 -Ofast -fopenmp -o naive naive.c
