TARGETS=ringmaster player

CC=g++
CFLAGS= -ggdb3 -std=c++11

all: $(TARGETS)

ringmaster: ringmaster.cpp potato.h ringmaster.h
	$(CC) $(CFLAGS) -o ringmaster ringmaster.cpp 

player: player.cpp potato.h player.h
	$(CC) $(CFLAGS) -o player player.cpp 

.PHONY:
	clean
clean:
	rm -rf *.o ringmaster player