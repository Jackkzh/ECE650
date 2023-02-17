#include "potato.h"
#include "connection_function.h"

int main(int argc, char *argv[]) {
    //get the port number
    const char *port = argv[2];
    const char *machine_name = argv[1];

    int player_fd = clientInit(machine_name, port);
    
}