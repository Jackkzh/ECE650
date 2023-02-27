#include <unistd.h>

#include "connection_function.h"
#include "potato.h"

int main(int argc, char *argv[]) {
    // check for the number of arguments steps
    if (argc != 4) {
        cerr << "Error: invalid number of arguments" << endl;
        return EXIT_FAILURE;
    }

    // get the port number
    const char *port = argv[1];
    int player_num = atoi(argv[2]);
    int hop_num = atoi(argv[3]);

    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << player_num << endl;
    cout << "Hops = " << hop_num << endl;

    // server inits socket and listen to the connection
    int socket_fd = serverStartConnection(port);

    // accept the connection from all the players
    vector<int> player_fd_list;
    vector<int> player_port_list;
    vector<char *> player_ip_list;
    clientJoinConnection(socket_fd, player_fd_list, player_port_list, player_ip_list, player_num);
    cout << "All players present, ready to play" << endl;
    // print out all the port numbers and ip addresses of the players
    for (int i = 0; i < player_num; i++) {
        cout << "Player " << i << " has IP address " << player_ip_list[i] << " and port number " << player_port_list[i] << endl;
    }


    playerConnect(player_fd_list, player_port_list, player_ip_list);



    // print our fdlist
    for (int i = 0; i < player_num; i++) {
        cout << "player " << i << " fd: " << player_fd_list[i] << endl;
    }

    // begin playing
    Potato potato(hop_num);

    srand(time(NULL) + player_num);
    int random_player = rand() % player_num;
    cout << "Ready to start the game, sending potato to player " << random_player << endl;
    send(player_fd_list[random_player], &potato, sizeof(potato), 0);

    // listen to the player who has the potato
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(player_fd_list[random_player], &read_fds);
    int max_fd = player_fd_list[random_player];
    int fd_num = select(max_fd + 1, &read_fds, NULL, NULL, NULL);


    


    return 0;
}