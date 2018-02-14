all: naive orig forced_unroll indexed unroll_indexed consecutive_write consecutive_write_byte consecutive_write_memcpy

naive: naive.c
	gcc -march=native -O3 -Ofast -fopenmp -o naive -std=c99 main.c naive.c

orig: main.c constants.h orig.c
	gcc -march=native -O3 -Ofast -fopenmp -o orig -std=c99 main.c orig.c

forced_unroll: main.c constants.h forced_unroll.c
	gcc -march=native -O3 -Ofast -fopenmp -o forced_unroll -std=c99 main.c forced_unroll.c

indexed: main.c constants.h indexed.c
	gcc -march=native -O3 -Ofast -fopenmp -o indexed -std=c99 main.c indexed.c

unroll_indexed: main.c constants.h unroll_indexed.c
	gcc -march=native -O3 -Ofast -fopenmp -o unroll_indexed -std=c99 main.c unroll_indexed.c

consecutive_write_byte: main.c constants.h consecutive_write_byte.c
	gcc -march=native -O3 -Ofast -fstrict-aliasing -fopenmp -o consecutive_write_byte -std=c99 main.c consecutive_write_byte.c

consecutive_write_memcpy: main.c constants.h consecutive_write_memcpy.c
	gcc -march=native -O3 -Ofast -fstrict-aliasing -fopenmp -o consecutive_write_memcpy -std=c99 main.c consecutive_write_memcpy.c

consecutive_write: main.c constants.h consecutive_write.c
	gcc -march=native -O3 -Ofast -fstrict-aliasing  -fopenmp -o consecutive_write -std=c99 main.c consecutive_write.c

clean:
	rm -f naive orig forced_unroll indexed unroll_indexed consecutive_write consecutive_write_byte consecutive_write_memcpy

time: all
	# echo naive                && ./naive 12 1536 50 # incorrect!
	echo orig                 && ./orig 12 1536 50
	echo forced_unroll        && ./forced_unroll 12 1536 50
	echo indexed              && ./indexed 12 1536 50
	echo unroll_indexed       && ./unroll_indexed 12 1536 50
	echo consecutive_write        && ./consecutive_write 12 1536 50
	echo consecutive_write_byte   && ./consecutive_write_byte 12 1536 50
	echo consecutive_write_memcpy && ./consecutive_write_memcpy 12 1536 50
