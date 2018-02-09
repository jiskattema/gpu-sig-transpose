



all: naive

naive: naive.c
	g++ -march=native -O3 -Ofast -fopenmp -o naive naive.c



clean:
	rm naive
