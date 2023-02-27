#include "ringmaster.h"

#include <chrono>
#include <ctime>
#include <iomanip>
/**
 * server start a connection on the given port and listens for connections.
 * @param port the port number to listen on as a server
 * @return the file descriptor for the socket that will listen for connections
 */
void RingMaster::initServer() {
    // cout << "Server Start Connection" << endl;

    int status;
    struct addrinfo client_info;
    struct addrinfo *client_info_list;
    const char *hostname = NULL;

    memset(&client_info, 0, sizeof(client_info));
    client_info.ai_family = AF_UNSPEC;
    client_info.ai_socktype = SOCK_STREAM;
    client_info.ai_flags = AI_PASSIVE;

    // initialize the client_info_list, which is a linked list of addrinfo structs
    status = getaddrinfo(hostname, port.c_str(), &client_info, &client_info_list);
    if (status != 0) {
        cerr << " Error: cannot get address info for host" << endl;
        cerr << gai_strerror(status) << endl;
        exit(EXIT_FAILURE);
    }

    // listen_fd is the file descriptor for the socket that will listen for connections
    listen_fd = socket(client_info_list->ai_family,
                       client_info_list->ai_socktype,
                       client_info_list->ai_protocol);
    if (listen_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    // setsockopt is used to reuse the port
    status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    status = bind(listen_fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot set socket option" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    status = listen(listen_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(client_info_list);
}

/**
 * allow Client join the connection. This function pass in
 * the listen_fd of the server that listen to the connection,
 * accept the all the connections and return the list of socket_fds of the client.
 */
void RingMaster::clientJoinConnection() {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int new_fd;

    // Initialize all connect() for every player with the server which listens to the connection
    // curr_player is the ID of each player(can retrieve through vector)
    for (int curr_player = 0; curr_player < player_num; curr_player++) {
        new_fd = accept(listen_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (new_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            exit(EXIT_FAILURE);
        }

        // tell the player which ID it is
        send(new_fd, &curr_player, sizeof(curr_player), 0);
        // send the player the total number of players
        send(new_fd, &player_num, sizeof(player_num), 0);
        player_fd_list.push_back(new_fd);
        cout << "Player " << curr_player << " is ready to play" << endl;

        // receive the message from the client of its port and host
        char client_host[4096];
        memset(client_host, 0, sizeof(client_host));

        int client_port;
        recv(new_fd, &client_port, sizeof(client_port), 0);
        //int len2 = recv(new_fd, client_host, sizeof(client_host), 0);
        //client_host[len2] = '\0';

        player_port_list.push_back(client_port);
        //player_ip_list.push_back(client_host);
    }
}

/**
 * send the port and host info to the player smaller than curr, if curr = 0, then send to the last player
 */
void RingMaster::playerConnect() {
    for (int curr = 0; curr < player_num; curr++) {
        // char player_host[128] = {0};
        // int len = strlen(player_ip_list[curr]);
        // strncpy(player_host, player_ip_list[curr], len);
        int prev = (curr - 1 + player_num) % player_num;
        send(player_fd_list[prev], &(player_port_list[curr]), sizeof(player_port_list[curr]), 0);
        // send(player_fd_list[prev], player_host, len, 0);
        // cout << a << endl;
    }
}

/**
 * print the trace of the potato
 * @param potato the potato that is passed around
*/
void RingMaster::printTrace(Potato &potato) {
    for (int i = 0; i < potato.count; i++) {
        cout << potato.playerID[i];
        if (i != potato.count - 1) {
            cout << ",";
        }
    }
    cout << endl;
}

/**
 * play the game
 */
void RingMaster::playGame() {
    Potato potato(hop_num);

    srand((unsigned int)time(NULL) + player_num);
    int rand_player = rand() % player_num;
    if (hop_num > 0) {
        int b = send(player_fd_list[rand_player], &potato, sizeof(potato), 0);
        cout << "Ready to start the game, sending potato to player " << rand_player << endl;

        fd_set read_fds;
        FD_ZERO(&read_fds);
        for (int i = 0; i < player_num; i++) {
            FD_SET(player_fd_list[i], &read_fds);
        }
        // find the biggest fd
        int max_fd = 0;
        for (int i = 0; i < player_num; i++) {
            if (player_fd_list[i] > max_fd) {
                max_fd = player_fd_list[i];
            }
        }

        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        for (int i = 0; i < player_num; i++) {
            if (FD_ISSET(player_fd_list[i], &read_fds)) {
                recv(player_fd_list[i], &potato, sizeof(potato), MSG_WAITALL);
                for (int i = 0; i < player_num; i++) {
                    send(player_fd_list[i], &potato, sizeof(potato), 0);
                }
                break;
            }
        }
    }

    cout << "Trace of potato:" << endl;
    printTrace(potato);
}

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

    string my_port = string(port);

    RingMaster *ringmaster = new RingMaster(my_port, player_num, hop_num);
    ringmaster->initServer();
    ringmaster->clientJoinConnection();
    ringmaster->playerConnect();
    ringmaster->playGame();
    delete ringmaster;
    return EXIT_SUCCESS;
}