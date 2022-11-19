all: src/pilo.c
	gcc -Wall -Wextra -std=c11 -pedantic -O3 -o bin/pilo src/pilo.c
