#include "player.h"

#include <sstream>

/**
 * Client inits socket and connects to the server with the given host and port.
 */
void Player::initClient(int &fd, string &host, string &port) {
    struct addrinfo client_info;
    struct addrinfo *client_info_list;
    int status;
    memset(&client_info, 0, sizeof(client_info));
    client_info.ai_family = AF_UNSPEC;
    client_info.ai_socktype = SOCK_STREAM;

    // initialize the client_info_list, which is a linked list of addrinfo structs
    status = getaddrinfo(host.c_str(), port.c_str(), &client_info, &client_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host " << endl;
        cerr << gai_strerror(status) << endl;
        exit(EXIT_FAILURE);
    }

    // listen_fd is the file descriptor for the socket that will listen for connections
    fd = socket(client_info_list->ai_family,
                client_info_list->ai_socktype,
                client_info_list->ai_protocol);
    if (fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        exit(EXIT_FAILURE);
    }

    // connect to the server(ringmaster)
    status = connect(fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(client_info_list);
}

/**
 * server start a connection on the given port and listens for connections.
 */
void Player::initServer() {
    int status;
    struct addrinfo client_info;
    struct addrinfo *client_info_list;
    const char *hostname = NULL;

    memset(&client_info, 0, sizeof(client_info));
    client_info.ai_family = AF_UNSPEC;
    client_info.ai_socktype = SOCK_STREAM;
    client_info.ai_flags = AI_PASSIVE;

    // initialize the client_info_list, which is a linked list of addrinfo structs
    status = getaddrinfo(hostname, my_port.c_str(), &client_info, &client_info_list);
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
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    // setsockopt is used to reuse the port
    status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    status = bind(listen_fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot set socket option" << endl;
        exit(EXIT_FAILURE);
    }

    status = listen(listen_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(client_info_list);
}

/**
 * Player accepts the connection from the left neighbor
 */
void Player::acceptConnection() {
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    server_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_size);
    if (server_fd == -1) {
        cerr << "Error: cannot accept connection on socket" << endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * Player receives the number of players and its id from the ringmaster
 * and set the hostname and port for itself.
 */
void Player::getMyInfo() {
    int id;
    int num;
    recv(connect_fd, &id, sizeof(id), 0);
    recv(connect_fd, &num, sizeof(num), 0);
    my_id = id;
    player_num = num;
    cout << "Connected as player " << my_id << " out of " << player_num << " total players" << endl;

    //initServer();
    char my_host[256];
    memset(my_host, 0, sizeof(my_host));
    int status = gethostname(my_host, 256);
    if (status == -1) {
        cerr << "Error: cannot get my hostname" << endl;
        exit(EXIT_FAILURE);
    }

    my_hostname = string(my_host);
    int my_port_int = atoi(ringmaster_port.c_str()) + my_id + 1;

    stringstream ss;
    ss << my_port_int;
    my_port = ss.str();
    initServer();
    int len2 = send(connect_fd, &my_port_int, sizeof(my_port_int), 0);
    //int len1 = send(connect_fd, &my_host, sizeof(my_host), 0);
}

/**
 * Player sends its hostname and port to the ringmaster
 */
void Player::sendMyInfo() {
    // change my_port to int
    int my_port_int = atoi(my_port.c_str());
    const char *my_hostname_c = my_hostname.c_str();
    int len2 = send(connect_fd, &my_port_int, sizeof(my_port_int), 0);
    int len1 = send(connect_fd, my_hostname_c, strlen(my_hostname_c), 0);
}

/**
 * Player receoves the neighbor's host and port from the ringmaster
 */
void Player::getNeighborInfo() {
    int neighbor_port;
    // char neighbor_host[1024];
    // memset(neighbor_host, 0, sizeof(neighbor_host));
    recv(connect_fd, &neighbor_port, sizeof(neighbor_port), 0);
    // convert neighbor_port to string
    stringstream ss;
    ss << neighbor_port;
    right_neighbor_port = ss.str();
    // recv(connect_fd, neighbor_host, sizeof(neighbor_host), 0);
    // right_neighbor_hostname = string(neighbor_host);
    // cout << "right neighbor hostname: " << right_neighbor_hostname << endl;
}

/**
 * Player receives the potato and pass it
 */
void Player::passPotato() {
    srand((unsigned int)time(NULL) + my_id);
    vector<int> listen_list = {server_fd, client_fd, connect_fd};
    Potato potato;
    fd_set read_fds;

    int max_fd = -1;
    for (size_t i = 0; i < listen_list.size(); i++) {
        if (listen_list[i] > max_fd) {
            max_fd = listen_list[i];
        }
    }
    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);   // as a server, listen to the left neighbor
        FD_SET(client_fd, &read_fds);   // as a client, listen to the right neighbor
        FD_SET(connect_fd, &read_fds);  // as a client, listen to the ringmaster

        int fd_num = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (fd_num == -1) {
            cerr << "Error: cannot select" << endl;
            exit(EXIT_FAILURE);
        } else if (fd_num == 0) {
            cerr << "Error: select timeout" << endl;
        } else {
            for (size_t i = 0; i < listen_list.size(); i++) {
                if (FD_ISSET(listen_list[i], &read_fds)) {
                    recv(listen_list[i], &potato, sizeof(potato), 0);
                    cout << "---*****---" << endl;
                    if (i == 0) {
                        cout << "recevied from " << "left neighbor" << endl;
                    } else if (i == 1) {
                        cout << "recevied from " << "right neighbor" << endl;
                    } else {
                        cout << "recevied from " << "ringmaster" << endl;
                    }
                    cout << "potato.hops: " << potato.hops << endl;

                    break;
                }
            }
        }
        if (potato.hops == 0) {
            // cout << "getting -1 " << endl;
            return;
        }
        if (potato.hops == 1) {
            cout << "I'm it" << endl;
            potato.playerID[potato.count] = my_id;
            potato.hops--;
            potato.count++;
            send(connect_fd, &potato, sizeof(potato), 0);
            continue;
        } else {
            cout << potato.count << endl;
            potato.playerID[potato.count] = my_id;
            potato.hops--;
            potato.count++;

            int rand_player = rand() % 2;

            if (rand_player == 0) {
                send(server_fd, &potato, sizeof(potato), 0);
                int dest_id = my_id == 0 ? player_num - 1 : my_id - 1;
                cout << "Sending potato to " << dest_id << endl;
            } else {
                int dest_id = my_id == player_num - 1 ? 0 : my_id + 1;
                send(client_fd, &potato, sizeof(potato), 0);
                cout << "Sending potato to " << dest_id << endl;
            }
            cout << "---*****---" << endl;
        }
    }
}

int main(int argc, char *argv[]) {
    const char *ring_name = argv[1];
    const char *ring_port = argv[2];
    string ring_name_str(ring_name);
    string ring_port_str(ring_port);

    Player *player = new Player(ring_name_str, ring_port_str);
    
    player->initClient(player->connect_fd, player->ringmaster_host, player->ringmaster_port);
    
    player->getMyInfo();
    //player->initServer();
    player->getNeighborInfo();
    player->initClient(player->client_fd, player->my_hostname, player->right_neighbor_port);
    player->acceptConnection();
    player->passPotato();
    delete player;
    return 0;
}