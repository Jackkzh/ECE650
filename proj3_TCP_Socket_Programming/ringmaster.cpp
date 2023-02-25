#include "ringmaster.h"
#include "potato.h"

/**
 * server start a connection on the given port and listens for connections.
 * @param port the port number to listen on as a server
 * @return the file descriptor for the socket that will listen for connections
 */
void RingMaster::initServer(const char *port) {
    cout << "Server Start Connection" << endl;

    int status;
    struct addrinfo client_info;
    struct addrinfo *client_info_list;
    const char *hostname = NULL;

    memset(&client_info, 0, sizeof(client_info));
    client_info.ai_family = AF_UNSPEC;
    client_info.ai_socktype = SOCK_STREAM;
    client_info.ai_flags = AI_PASSIVE;

    // initialize the client_info_list, which is a linked list of addrinfo structs
    status = getaddrinfo(hostname, port, &client_info, &client_info_list);
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
    int status;
    int i;

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
        int len1 = recv(new_fd, &client_port, sizeof(client_port), 0);
        int len2 = recv(new_fd, client_host, sizeof(client_host), 0);
        client_host[len2] = '\0';

        cout << "player's port: " << client_port << endl;
        cout << "player's host: " << client_host << endl;
        
        player_port_list.push_back(client_port);
        player_ip_list.push_back(client_host);

        cout << "player: " << player_port_list[curr_player] << endl;

    }
    // cout << "succfully connected to all players: " << curr_player << endl;
}

/**
 * send the port and host info to the player smaller than curr, if curr = 0, then send to the last player
*/
void RingMaster::playerConnect() {
    for (int curr = 0; curr < player_num; curr++) {
        int server_port = player_port_list[curr];
        char *server_host = player_ip_list[curr];
        size_t host_len = strlen(server_host) + 1;
        //cout << server_host << endl;

        int prev = (curr - 1 + player_num) % player_num;
        send(player_fd_list[prev], &(player_port_list[curr]), sizeof(player_port_list[curr]), 0);
        //cout << "player: " << player_port_list[curr] << endl;
        send(player_fd_list[prev], server_host, host_len, 0);

    }
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

    


    return 0;
}