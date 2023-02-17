#include <unistd.h>

#include "connection_function.h"
#include "potato.h"

int main(int argc, char *argv[]) {
    // attributes for a player

    char my_hostname[256];
    // char my_port[256];  // not used yet
    int my_id;

    // get the port number
    const char *port = argv[2];
    const char *machine_name = argv[1];

    // player receives its id from the ringmaster
    int player_fd = clientInit(machine_name, port);

    recv(player_fd, &my_id, sizeof(my_id), 0);

    // player sends its host and port to the ringmaster
    int status = gethostname(my_hostname, 256);
    if (status == -1) {
        cerr << "Error: cannot get hostname" << endl;
        return EXIT_FAILURE;
    }
    // player acts as a server can make connections with its left neighbor
    int my_port = atoi(port) + my_id + 1;
    // convert int to const char*
    const char *my_port_str = std::to_string(my_port).c_str();
    const int size = strlen(my_port_str) + 1;
    char my_port2[size];
    strcpy(my_port2, my_port_str);
    send(player_fd, &my_port, sizeof(my_port), 0);
    int size2 = send(player_fd, &my_hostname, sizeof(my_hostname), 0);


    char client_host[1024];
    int client_port;
    int len1 = recv(player_fd, &client_port, sizeof(client_port), 0);
    int len2 = recv(player_fd, client_host, sizeof(client_host), 0);
    client_host[len2] = '\0';
    cout << "len2: " << len2 << endl;
    cout << "client_port: " << client_port << endl;
    cout << "client_host: " << client_host << endl;
    int server_fd = serverStartConnection(my_port2);
    int client_fd = clientInit(client_host, to_string(client_port).c_str());
    // player acts as a client and connects to its right neighbor
    // how do i know the port number of the right neighbor?

    //check if the player receives the potato
    

}