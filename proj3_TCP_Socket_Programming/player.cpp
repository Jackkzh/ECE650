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
    // cout << "Connected to " << ringmaster_host << " on port " << ringmaster_port << endl;

    freeaddrinfo(client_info_list);
}

/**
 * server start a connection on the given port and listens for connections.
 */
void Player::initServer() {
    // cout << "start as a server" << endl;

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
    // cout << "my id is " << my_id << endl;
    // cout << "player_num is " << player_num << endl;

    char my_host[256];
    memset(my_host, 0, sizeof(my_host));
    int status = gethostname(my_host, 256);
    if (status == -1) {
        cerr << "Error: cannot get my hostname" << endl;
        exit(EXIT_FAILURE);
    }

    my_hostname = string(my_host);
    int my_port_int = atoi(ringmaster_port.c_str()) + my_id + 1;
    // turn my_port to const char *
    stringstream ss;
    ss << my_port_int;
    my_port = ss.str();
    // cout << "my hostname is " << my_host << endl;
    // cout << "my port is " << my_port << endl;

    // int my_port_int = atoi(my_port.c_str());
    // cout << "my_port_int is " << my_port_int << endl;
    int len2 = send(connect_fd, &my_port_int, sizeof(my_port_int), 0);
    // cout << "len2 is " << len2 << endl;

    int len1 = send(connect_fd, &my_host, sizeof(my_host), 0);
    // cout << "len1 is " << len1 << endl;
}

/**
 * Player sends its hostname and port to the ringmaster
 */
void Player::sendMyInfo() {
    // change my_port to int
    int my_port_int = atoi(my_port.c_str());
    // cout << "my_port_int is " << my_port_int << endl;

    const char *my_hostname_c = my_hostname.c_str();
    int len2 = send(connect_fd, &my_port_int, sizeof(my_port_int), 0);
    // cout << "len2 is " << len2 << endl;

    int len1 = send(connect_fd, my_hostname_c, strlen(my_hostname_c), 0);
    // cout << "len1 is " << len1 << endl;
}

/**
 * Player receoves the neighbor's host and port from the ringmaster
 */
void Player::getNeighborInfo() {
    int neighbor_port;
    char neighbor_host[1024];
    memset(neighbor_host, 0, sizeof(neighbor_host));
    recv(connect_fd, &neighbor_port, sizeof(neighbor_port), 0);
    // convert neighbor_port to string
    stringstream ss;
    ss << neighbor_port;
    right_neighbor_port = ss.str();
    // cout << "neighbour port " << right_neighbor_port << endl;
    recv(connect_fd, neighbor_host, sizeof(neighbor_host), 0);
    right_neighbor_hostname = string(neighbor_host);
    // cout << "neighbour ip " << right_neighbor_hostname << endl;
}

/**
 * Player receives the potato and pass it
 */
void Player::passPotato() {
    vector<int> listen_list = {server_fd, client_fd, connect_fd};
    // cout << "print listen_list" << endl;
    // for (size_t i = 0; i < listen_list.size(); i++) {
    //     cout << listen_list[i] << endl;
    // }
    Potato potato;
    fd_set read_fds;
    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);   // as a server, listen to the left neighbor
        FD_SET(client_fd, &read_fds);   // as a client, listen to the right neighbor
        FD_SET(connect_fd, &read_fds);  // as a client, listen to the ringmaster

        // get the max fd
        int max_fd = -1;
        for (size_t i = 0; i < listen_list.size(); i++) {
            if (listen_list[i] > max_fd) {
                max_fd = listen_list[i];
            }
        }

        // cout << "before select" << endl;
        int fd_num = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        // bool is_potato = false;
        // int lenn;
        // cout << "after select" << endl;
        if (fd_num == -1) {
            cerr << "Error: cannot select" << endl;
            exit(EXIT_FAILURE);
        } else if (fd_num == 0) {
            cerr << "Error: select timeout" << endl;
        } else {
            //cout << "before for loop " << endl;
            for (size_t i = 0; i < listen_list.size(); i++) {
                if (FD_ISSET(listen_list[i], &read_fds)) {
                    sleep(1);
                    // cout << "listen_list[i]: " << listen_list[i] << endl;
                    // int recv_len = 0;
                    int rcv = recv(listen_list[i], &potato, sizeof(potato), MSG_WAITALL);
                    // cout << "rcv: " << rcv << endl;
                    break;
                    // while (true) {
                    //     int rcv = recv(listen_list[i], &potato, sizeof(potato), MSG_WAITALL);
                    //     recv_len += rcv;
                    //     if (rcv == 0) {
                    //         break;
                    //     }
                    // }
                    // is_potato = true;
                }
            }
        }
        if (potato.hops == -1) {
            break;
        }
        // cout << "----------------------------" << endl;
        // cout << "player " << my_id << " receives potato" << endl;

        // cout << "potaot.hops: " << potato.hops << endl;
        // cout << "----------------------------" << endl;
        sleep(1);
        if (potato.hops == 1) {
            cout << "I'm it" << endl;
            potato.playerID[potato.count] = my_id;
            potato.hops--;
            potato.count++;
            send(connect_fd, &potato, sizeof(potato), 0);
            // break;
        } else {
            //  pass potato to the left neighbor or right neighbor
            // cout << "**************" << endl;
            // cout << "Player " << my_id << " received potato" << endl;
            // cout << "**************" << endl;
            // potato.playerID[potato.count] = my_id;
            //cout << "count: " << potato.count << endl;
            //cout << my_id;
            potato.playerID[potato.count] = my_id;
            //cout << potato.playerID[potato.count] << endl;
            potato.hops--;
            potato.count++;
            // cout << "count: " << potato.count << endl;

            // send(client_fd, &potato, sizeof(potato), 0);

            // int rand_player = rand() % 2;
            srand((unsigned int)time(NULL) + my_id);
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
        }
    }
    close(server_fd);
    close(client_fd);
    close(connect_fd);
    close(listen_fd);
}

int main(int argc, char *argv[]) {
    const char *ring_name = argv[1];
    const char *ring_port = argv[2];
    string ring_name_str(ring_name);
    string ring_port_str(ring_port);

    Player player(ring_name_str, ring_port_str);

    player.initClient(player.connect_fd, player.ringmaster_host, player.ringmaster_port);
    //cout << "connected to the ringmaster" << endl;
    player.getMyInfo();
    // player.sendMyInfo();
    player.initServer();

    player.getNeighborInfo();
    player.initClient(player.client_fd, player.right_neighbor_hostname, player.right_neighbor_port);
    player.acceptConnection();
    //cout << "connected to the right neighbor" << endl;

    player.passPotato();

    // vector<int> listen_list = {server_fd, client_fd, player_fd};
    // Potato potato;
    // fd_set read_fds;
    // // cout << "FD SIZE" << FD_SETSIZE << endl;
    // while (true) {
    //     FD_ZERO(&read_fds);
    //     FD_SET(server_fd, &read_fds);  // as a server, listen to the left neighbor
    //     FD_SET(client_fd, &read_fds);  // as a client, listen to the right neighbor
    //     FD_SET(player_fd, &read_fds);  // as a client, listen to the ringmaster

    //     // get the max fd
    //     int max_fd = -1;
    //     for (int i = 0; i < listen_list.size(); i++) {
    //         if (listen_list[i] > max_fd) {
    //             max_fd = listen_list[i];
    //         }
    //     }

    //     int fd_num = select(1000, &read_fds, NULL, NULL, NULL);
    //     bool is_potato = false;
    //     int lenn;
    //     if (fd_num == -1) {
    //         cerr << "Error: cannot select" << endl;
    //         return EXIT_FAILURE;
    //     } else if (fd_num == 0) {
    //         cerr << "Error: select timeout" << endl;
    //     } else {
    //         for (int i = 0; i < listen_list.size(); i++) {
    //             if (FD_ISSET(listen_list[i], &read_fds)) {
    //                 cout << "listen_list[i]: " << listen_list[i] << endl;
    //                 int left = 0;
    //                 int recv_len = 0;
    //                 while (true) {
    //                     int rcv = recv(listen_list[i], &potato, sizeof(potato), MSG_WAITALL);
    //                     recv_len += rcv;
    //                     if (rcv == 0) {
    //                         break;
    //                     }
    //                 }
    //                 is_potato = true;
    //                 // lenn = recv(listen_list[i], &potato, sizeof(potato), MSG_WAITALL);
    //                 // if (lenn == 0) {
    //                 //     continue;
    //                 // }
    //                 // cout << "potato hop " << potato.hops << endl;
    //             }
    //         }
    //     }
    //     // if (lenn == 0) {
    //     //     break;
    //     // }

    //     // cout << "is potato: " << is_potato << endl;
    //     // cout << "after recv count" << potato.count << endl;
    //     cout << "player " << my_id << " receives potato" << endl;
    //     cout << "potaot.hops: " << potato.hops << endl;
    //     cout << "potaot.count: " << potato.count << endl;

    //     if (potato.hops == 0) {
    //         cout << "I'm it" << endl;
    //         potato.playerID[potato.count] = my_id;
    //         send(player_fd, &potato, sizeof(potato), MSG_WAITALL);
    //         break;
    //     } else {
    //         // cout << "hi" << endl;
    //         //  receive potato from the left neighbor or right neighbor
    //         cout << "Player " << my_id << " received potato" << endl;
    //         potato.playerID[potato.count] = my_id;
    //         potato.hops--;
    //         // cout << "count: " << potato.count << endl;

    //         // send(client_fd, &potato, sizeof(potato), 0);

    //         // int rand_player = rand() % 2;
    //         int rand_player = 0;
    //         if (rand_player == 0) {
    //             send(server_fd, &potato, sizeof(potato), 0);
    //             int dest_id = my_id == 0 ? player_num - 1 : my_id - 1;
    //             cout << "Sending potato to " << dest_id << endl;
    //         } else {
    //             int dest_id = my_id == player_num - 1 ? 0 : my_id + 1;
    //             send(client_fd, &potato, sizeof(potato), 0);
    //             cout << "Sending potato to " << dest_id << endl;
    //         }
    //     }
    // }
    return 0;
}

// fd_num is the socket descriptor that has data to read
// send potato to the right neighbor or left neighbor

// int my_port_int = atoi(player.my_port.c_str());

// int sent_len1 = send(player.connect_fd, &my_port_int, sizeof(my_port_int), 0);
// int sent_len2 = send(player.connect_fd, player.my_hostname.c_str(), strlen(player.my_hostname.c_str()), 0);

// char client_host[1024];
// // fill client_host with 0
// memset(client_host, 0, sizeof(client_host));
// int client_port;

// int len1 = recv(player_fd, &client_port, sizeof(client_port), 0);
// int len2 = recv(player_fd, client_host, sizeof(client_host), 0);
// client_host[len2] = '\0';
// cout << "my neighbour's port: " << client_port << endl;
// cout << "my neighbour's host: " << client_host << endl;

// int client_fd = clientInit(machine_name, to_string(client_port).c_str());
// cout << "ring clientfd " << client_fd << endl;
// // player connects to its left neighbor as a server
// struct sockaddr_storage socket_addr;
// socklen_t socket_addr_len = sizeof(socket_addr);
// int server_fd = accept(server_listen_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);

// struct sockaddr_in client_addr;
// socklen_t client_addr_len = sizeof(client_addr);
// cout << "before getpeername" << endl;
// if (getpeername(client_fd, (struct sockaddr *)&client_addr, &client_addr_len) == 0) {
//     char client_ip[INET_ADDRSTRLEN];
//     cout << "1" << endl;
//     inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
//     cout << "2" << endl;
//     int client_port = ntohs(client_addr.sin_port);
//     cout << "3" << endl;
//     cout << "client ip: " << client_ip << endl;
//     cout << "4" << endl;
//     cout << "client port: " << client_port << endl;

// } else {
//     perror("getpeername");
// }

// cout << "server_fd: " << server_fd << endl;
// if (server_fd == -1) {
//     cerr << "Error: cannot accept connection" << endl;
//     return EXIT_FAILURE;
// }

// // cout << "player " << my_id << " connects to its left neighbor" << endl;

// // listen to both the left and right neighbors for a potato

// // cout << "in my listen list, i have:" << endl;
// // for (int i = 0; i < listen_list.size(); i++) {
// //     cout << "listen_list: " << listen_list[i] << endl;
// // }