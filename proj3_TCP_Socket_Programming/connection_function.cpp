#include "connection_function.h"


/**
 * server start a connection on the given port and listens for connections.
 * @param port the port number to listen on as a server
 * @return the file descriptor for the socket that will listen for connections
 */
int serverStartConnection(const char *port) {
    cout << "Server Start Connection" << endl;
    int status;
    int listen_fd;
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
        cerr << "Error: cannot get address info for host: " << endl;
        cerr << gai_strerror(status) << endl;
        return -1;
    }

    // listen_fd is the file descriptor for the socket that will listen for connections
    listen_fd = socket(client_info_list->ai_family,
                       client_info_list->ai_socktype,
                       client_info_list->ai_protocol);
    if (listen_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }

    int yes = 1;
    // setsockopt is used to reuse the port
    status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    status = bind(listen_fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot set socket option" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }

    status = listen(listen_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }

    freeaddrinfo(client_info_list);
    return listen_fd;
}

/**
 * Client inits socket and connects to the server with the given host and port.
 */
int clientInit(const char *host, const char *port) {
    struct addrinfo client_info;
    struct addrinfo *client_info_list;
    int status;
    int client_fd;

    memset(&client_info, 0, sizeof(client_info));
    client_info.ai_family = AF_UNSPEC;
    client_info.ai_socktype = SOCK_STREAM;

    // initialize the client_info_list, which is a linked list of addrinfo structs
    status = getaddrinfo(host, port, &client_info, &client_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host: " << host << endl;
        cerr << gai_strerror(status) << endl;
        return -1;
    }

    // listen_fd is the file descriptor for the socket that will listen for connections
    client_fd = socket(client_info_list->ai_family,
                       client_info_list->ai_socktype,
                       client_info_list->ai_protocol);
    if (client_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << host << "," << port << ")" << endl;
        return -1;
    }

    // connect to the server
    status = connect(client_fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        cerr << "  (" << host << "," << port << ")" << endl;
        return -1;
    }
    cout << "Connected to " << host << " on port " << port << endl;

    freeaddrinfo(client_info_list);
    // cout << "finished clientInit" << endl;
    return client_fd;
}

/**
 * allow Client join the connection. This function pass in
 * the listen_fd of the server that listen to the connection,
 * accept the all the connections and return the list of socket_fds of the client.
 */
void clientJoinConnection(int listen_fd, vector<int> &player_fd_list,
                                 vector<int> &player_port_list, vector<char *> &player_ip_list, int player_num) {
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

void playerConnect(vector<int> &player_fd_list, vector<int> &player_port_list, vector<char *> &player_ip_list) {
    int player_num = player_fd_list.size();
    for (int curr = 0; curr < player_num; curr++) {
        int server_port = player_port_list[curr];
        char *server_host = player_ip_list[curr];
        size_t host_len = strlen(server_host) + 1;
        cout << server_host << endl;
        // sending the port and host info to the player smaller than curr, if curr = 0, then send to the last player
        int prev = (curr - 1 + player_num) % player_num;
        send(player_fd_list[prev], &(player_port_list[curr]), sizeof(player_port_list[curr]), 0);
        cout << "player: " << player_port_list[curr] << endl;
        send(player_fd_list[prev], server_host, host_len, 0);

    }
}

// void playerRingConnect(char *client_host, int &client_port, int &server_fd, int player_fd, char * my_port) {
//     int len1 = recv(player_fd, &client_port, sizeof(client_port), 0);
//     int len2 = recv(player_fd, client_host, sizeof(client_host), 0);
//     client_host[len2] = '\0';
//     cout << "len2: " << len2 << endl;
//     cout << "client_port: " << client_port << endl;
//     cout << "client_host: " << client_host << endl;
//     int server_fd = serverStartConnection(my_port);
//     int client_fd = clientInit(client_host, to_string(client_port).c_str());
// }

