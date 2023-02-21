#include <unistd.h>

#include "connection_function.h"
#include "potato.h"
// add header for ntop
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    // attributes for a player
    char my_hostname[256];
    memset(my_hostname, 0, sizeof(my_hostname));
    // char my_port[256];  // not used yet
    int my_id;       // player's id
    int player_num;  // total number of players

    // get the port number
    const char *port = argv[2];
    const char *machine_name = argv[1];

    // player receives its id from the ringmaster
    int player_fd = clientInit(machine_name, port);

    struct sockaddr_storage local_address;
    socklen_t address_length = sizeof(local_address);
    if (getsockname(player_fd, (struct sockaddr *)&local_address, &address_length) == -1) {
        std::cerr << "Error getting socket name\n";
        return 1;
    }

    char address_buffer[INET_ADDRSTRLEN];
    int my_port2;

    struct sockaddr_in *address = (struct sockaddr_in *)&local_address;
    inet_ntop(AF_INET, &(address->sin_addr), address_buffer, INET_ADDRSTRLEN);
    my_port2 = ntohs(address->sin_port);

    std::cout << "Local address: " << address_buffer << '\n';
    std::cout << "Local port: " << port << '\n';
    std::cout << "Local using my_port2: " << my_port2 << '\n';

    recv(player_fd, &my_id, sizeof(my_id), 0);
    recv(player_fd, &player_num, sizeof(player_num), 0);

    cout << "my id: " << my_id << endl;
    cout << "player num: " << player_num << endl;

    // player sends its host and port to the ringmaster
    int status = gethostname(my_hostname, 256);
    cout << "my hostname: " << my_hostname << endl;
    if (status == -1) {
        cerr << "Error: cannot get hostname" << endl;
        return EXIT_FAILURE;
    }
    // player acts as a server can make connections with its left neighbor
    int my_port_int = atoi(port) + my_id + 1;
    // convert int to const char*
    const char *my_port = to_string(my_port_int).c_str();
    // const int size = strlen(my_port_str) + 1;
    // char my_port2[size];
    // strcpy(my_port2, my_port_str);
    send(player_fd, &my_port_int, sizeof(my_port_int), 0);
    int size2 = send(player_fd, &my_hostname, sizeof(my_hostname), 0);

    char client_host[1024];
    // fill client_host with 0
    memset(client_host, 0, sizeof(client_host));
    int client_port;

    int len1 = recv(player_fd, &client_port, sizeof(client_port), 0);
    int len2 = recv(player_fd, client_host, sizeof(client_host), 0);
    client_host[len2] = '\0';
    cout << "my neighbour's port: " << client_port << endl;
    cout << "my neighbour's host: " << client_host << endl;

    int server_listen_fd = serverStartConnection(my_port);
    int client_fd = clientInit(machine_name, to_string(client_port).c_str());

    // player connects to its left neighbor as a server
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int server_fd = accept(server_listen_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    cout << "before getpeername" << endl;
    if (getpeername(client_fd, (struct sockaddr *)&client_addr, &client_addr_len) == 0) {
        char client_ip[INET_ADDRSTRLEN];
        cout << "1" << endl;
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        cout << "2" << endl;
        int client_port = ntohs(client_addr.sin_port);
        cout << "3" << endl;
        cout << "client ip: " << client_ip << endl;
        cout << "4" << endl;
        cout << "client port: " << client_port << endl;

    } else {
        perror("getpeername");
    }

    cout << "server_fd: " << server_fd << endl;
    if (server_fd == -1) {
        cerr << "Error: cannot accept connection" << endl;
        return EXIT_FAILURE;
    }
    const char *msg = "hello";
    send(server_fd, msg, strlen(msg), 0);
    send(player_fd, msg, strlen(msg), 0);
    cout << "self: " << msg << endl;

    char hello[1024];
    memset(hello, 0, sizeof(hello));
    recv(client_fd, hello, sizeof(hello), 0);
    cout << "my neighbor server says: " << hello << endl;

    // cout << "player " << my_id << " connects to its left neighbor" << endl;

    // listen to both the left and right neighbors for a potato

    // cout << "in my listen list, i have:" << endl;
    // for (int i = 0; i < listen_list.size(); i++) {
    //     cout << "listen_list: " << listen_list[i] << endl;
    // }
    vector<int> listen_list = {server_fd, client_fd, player_fd};
    Potato potato;
    fd_set read_fds;
    // cout << "FD SIZE" << FD_SETSIZE << endl;
    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);  // as a server, listen to the left neighbor
        FD_SET(client_fd, &read_fds);  // as a client, listen to the right neighbor
        FD_SET(player_fd, &read_fds);  // as a client, listen to the ringmaster

        // get the max fd
        int max_fd = -1;
        for (int i = 0; i < listen_list.size(); i++) {
            if (listen_list[i] > max_fd) {
                max_fd = listen_list[i];
            }
        }
        int fd_num = select(1000, &read_fds, NULL, NULL, NULL);
        bool is_potato = false;
        int lenn;
        if (fd_num == -1) {
            cerr << "Error: cannot select" << endl;
            return EXIT_FAILURE;
        } else if (fd_num == 0) {
            cerr << "Error: select timeout" << endl;
        } else {
            for (int i = 0; i < listen_list.size(); i++) {
                if (FD_ISSET(listen_list[i], &read_fds)) {
                    cout << "listen_list[i]: " << listen_list[i] << endl;
                    int left = 0;
                    lenn = recv(listen_list[i], &potato, sizeof(potato), MSG_WAITALL);
                    cout << "lenn: " << lenn << endl;
                    break;
                    if (lenn == 0) {
                        continue;
                    }
                    // cout << "potato hop " << potato.hops << endl;

                    is_potato = true;
                    // break;
                }
            }
        }
        if (lenn == 0) {
            break;
        }
        // cout << "is potato: " << is_potato << endl;
        // cout << "after recv count" << potato.count << endl;
        cout << "player " << my_id << " receives potato" << endl;
        cout << "potaot.hops: " << potato.hops << endl;
        cout << "potaot.count: " << potato.count << endl;

        if (potato.hops == 0) {
            cout << "I'm it" << endl;
            potato.playerID[potato.count] = my_id;
            send(player_fd, &potato, sizeof(potato), MSG_WAITALL);
            break;
        } else {
            // cout << "hi" << endl;
            //  receive potato from the left neighbor or right neighbor
            cout << "Player " << my_id << " received potato" << endl;
            potato.playerID[potato.count] = my_id;
            potato.hops--;
            potato.count++;
            // cout << "count: " << potato.count << endl;

            // send(client_fd, &potato, sizeof(potato), 0);

            // int rand_player = rand() % 2;
            int rand_player = 0;
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

    // Potato potato;
    // vector<int> all_connected_fd{server_fd, client_fd, player_fd};
    // while (1) {
    //     fd_set readfds;
    //     FD_ZERO(&readfds);
    //     for (int i = 0; i < 3; i++) {
    //         FD_SET(all_connected_fd[i], &readfds);
    //     }
    //     select(100, &readfds, NULL, NULL, NULL);
    //     int len;
    //     for (int i = 0; i < 3; i++) {
    //         if (FD_ISSET(all_connected_fd[i], &readfds)) {
    //             cout << "listening on fd " << all_connected_fd[i] << endl;
    //             len = recv(all_connected_fd[i], &potato, sizeof(potato), MSG_WAITALL);
    //             cout << "len of potato is " << len << endl;
    //             break;
    //         }
    //     }

    //     // receive num_hops =0 potato from master or shut down signal 0 from other socket, shut down
    //     if (potato.hops == 0 || len == 0) {
    //         break;
    //     }
    //     // send potato to master
    //     else if (potato.hops == 1) {
    //         potato.hops--;
    //         potato.playerID[potato.count] = my_id;
    //         potato.count++;
    //         send(player_fd, &potato, sizeof(potato), 0);
    //         cout << "I'm it" << endl;
    //     }
    //     // send potato to neighbor
    //     else {
    //         potato.hops--;
    //         potato.playerID[potato.count] = my_id;
    //         potato.count++;
    //         int random = rand() % 2;
    //         if (random == 0) {
    //             send(server_fd, &potato, sizeof(potato), 0);
    //             int left_neighbor_id = (my_id + player_num - 1) % player_num;
    //             cout << "Sending potato to " << left_neighbor_id << endl;
    //         } else {
    //             send(client_fd, &potato, sizeof(potato), 0);
    //             int right_neighbor_id = (my_id + 1) % player_num;
    //             cout << "Sending potato to " << right_neighbor_id << endl;
    //         }
    //     }
    // }
    close(client_fd);
    close(server_fd);
    close(player_fd);
    return 0;
}

// fd_num is the socket descriptor that has data to read
// send potato to the right neighbor or left neighbor
