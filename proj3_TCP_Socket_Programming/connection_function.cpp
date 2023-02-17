#include "connection_function.h"

/**
 * Start a connection on the given port.
 */
int serverStartConnection(const char *port) {
    int status;
    int socket_fd;
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

    // socket_fd is the file descriptor for the socket that will listen for connections
    socket_fd = socket(client_info_list->ai_family,
                       client_info_list->ai_socktype,
                       client_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    } 

    int yes = 1;
    // setsockopt is used to reuse the port
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    status = bind(socket_fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot set socket option" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    } 

    status = listen(socket_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }  

    freeaddrinfo(client_info_list);
    return socket_fd;
}


/**
 * Client inits socket and connects to the server.
*/
int clientInit(const char *host, const char *port) {
    struct addrinfo client_info;
    struct addrinfo *client_info_list;
    int status;
    int socket_fd;

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

    // socket_fd is the file descriptor for the socket that will listen for connections
    socket_fd = socket(client_info_list->ai_family,
                       client_info_list->ai_socktype,
                       client_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << host << "," << port << ")" << endl;
        return -1;
    }

    // connect to the server
    status = connect(socket_fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        cerr << "  (" << host << "," << port << ")" << endl;
        return -1;
    }
    
    freeaddrinfo(client_info_list);
    return socket_fd;
}



/**
 * Client join the connection. This function pass in 
 * the socket_fd of the server that listen to the connection,
 * accept the all the connections and return the list of socket_fds of the client.
*/
vector<int> clientJoinConnection(int socket_fd, const char *port, vector<int> player_fd_list, int player_num) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int new_fd;
    int status;
    int i;

    // Initialize all connect() for every player with the server which listens to the connection
    // curr_player is the ID of each player(can retrieve through vector)
    for (int curr_player = 0; curr_player < player_num; curr_player++) {
        new_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (new_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            exit(EXIT_FAILURE);
        }
        player_fd_list.push_back(new_fd);
    }
    return player_fd_list;
}



