TARGETS=ringmaster player

all: $(TARGETS)

ringmaster: ringmaster.cpp connection_function.cpp potato.h 
	g++ -ggdb3 -o ringmaster ringmaster.cpp connection_function.cpp

player: player.cpp connection_function.cpp potato.h 
	g++ -ggdb3 -o player player.cpp connection_function.cpp

.PHONY:
	clean
clean:
	rm -rf *.o ringmaster player