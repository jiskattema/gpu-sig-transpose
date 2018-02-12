



all: naive

naive: naive.c
	gcc -march=native -O3 -Ofast -fopenmp -o naive -std=c99 naive.c



clean:
	rm naive
