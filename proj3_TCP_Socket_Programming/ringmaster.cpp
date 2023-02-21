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

    for (int i = 0; i < player_num; i++) {
        int neighbor_id = (i + 1) % player_num;
        int neighbor_port = player_port_list[neighbor_id];
        char neighbor_ip[100];
        memset(neighbor_ip, 0, sizeof(neighbor_ip));
        strcpy(neighbor_ip, player_ip_list[neighbor_id]);
        send(player_fd_list[i], &neighbor_port, sizeof(neighbor_port), 0);
        send(player_fd_list[i], &neighbor_ip, sizeof(neighbor_ip), 0);
    }

    // playerConnect(player_fd_list, player_port_list, player_ip_list);

    // print all players' fd
    // for (int i = 0; i < player_num; i++) {
    //     cout << "player " << i << " fd: " << player_fd_list[i] << endl;
    // }

    // now creates a potato and send it to a random player

    // print our fdlist
    for (int i = 0; i < player_num; i++) {
        cout << "player " << i << " fd: " << player_fd_list[i] << endl;
    }

    Potato potato(hop_num);
    int rand_player = rand() % player_num;
    cout << "Ready to start the game, sending potato to player " << rand_player << endl;

    char hello[1024];
    memset(hello, 0, sizeof(hello));
    recv(player_fd_list[0], hello, sizeof(hello), 0);
    cout << "myplayer0 says: " << hello << endl;

    char hello1[1024];
    memset(hello1, 0, sizeof(hello1));
    recv(player_fd_list[1], hello1, sizeof(hello1), 0);
    cout << "my player1 says: " << hello1 << endl;

    // potato.playerID[0] = rand_player;
    // potato.hops--;
    // potato.count++;
    // // send(player_fd_list[rand_player], &potato, sizeof(potato), 0);
    // Potato *ptr = &potato;
    // ssize_t bytes_sent;
    // size_t bytes_left = sizeof(potato);
    // cout << "bytes left: " << bytes_left << endl;
    // while (bytes_left > 0) {
    //     bytes_sent = send(player_fd_list[rand_player], ptr, bytes_left, 0);
    //     if (bytes_sent < 0) {
    //         return -1; // error occurred
    //     }
    //     bytes_left -= bytes_sent;
    //     cout << "bytes left: " << bytes_left << endl;
    //     ptr += bytes_sent;
    // }
}
