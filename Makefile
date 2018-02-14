



all: naive orig forced_unroll forced_unroll_mp

naive: naive.c
	gcc -march=native -O3 -Ofast -fopenmp -o naive -std=c99 main.c naive.c

orig: main.c constants.h orig.c
	gcc -march=native -O3 -Ofast -fopenmp -o orig -std=c99 main.c orig.c

forced_unroll: main.c constants.h forced_unroll.c
	gcc -march=native -O3 -Ofast -fopenmp -o forced_unroll -std=c99 main.c forced_unroll.c

indexed: main.c constants.h indexed.c
	gcc -march=native -O3 -Ofast -fopenmp -o indexed -std=c99 main.c indexed.c

clean:
	rm naive
