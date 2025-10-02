CC = gcc
FLAGS = -Wall
PROGS = tema2

build:
	$(CC) tema2.c -o $(PROGS) $(FLAGS)
	
clean:
	rm -f $(PROGS)