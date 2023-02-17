#include "potato.h"
#include "connection_function.h"


int main(int argc, char *argv[]) {
    // skip the check for the number of arguments steps

    // get the port number
    const char *port = argv[1];
    int player_num = atoi(argv[2]);
    int hop_num = atoi(argv[3]);

    // server inits socket and listen to the connection
    int socket_fd = serverStartConnection(port);

    // accept the connection from all the players
    vector<int> player_fd_list;
    player_fd_list = clientJoinConnection(socket_fd, player_fd_list, player_num);
    
    // player builds a ring 
    



}
