#ifndef PLAYER_H
#define PLAYER_H

#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "potato.h"

class Player {
   public:
    int connect_fd;  // socker fd with ringmaster
    int listen_fd;   // socket fd with left neighbor before connection
    int server_fd;   // socket fd with left neighbor
    int client_fd;   // socket fd with right neighbor
    int my_id;
    int player_num;

    string ringmaster_host;
    string ringmaster_port;
    string my_hostname;
    string my_port;
    string right_neighbor_hostname;  // this is the player's server
    string right_neighbor_port;      // this is the player's server

    Player() : connect_fd(-2), listen_fd(-2), server_fd(-2), client_fd(-2), my_id(-1), 
                player_num(-1), ringmaster_host(""), ringmaster_port(""), 
                my_hostname(""), my_port(""), right_neighbor_hostname(""), right_neighbor_port(""){};

    Player(string _ringmaster_host, string _ringmaster_port) : 
                                                               connect_fd(-2), listen_fd(-2), server_fd(-2), 
                                                               client_fd(-2), my_id(-1),
                                                               player_num(-1), ringmaster_host(_ringmaster_host),
                                                               ringmaster_port(_ringmaster_port), my_hostname(""), 
                                                               my_port(""), right_neighbor_hostname(""), 
                                                               right_neighbor_port(""){};
    void initClient(int &fd, string &host, string &port);
    void initServer();
    void acceptConnection();
    void getMyInfo();
    void sendMyInfo();
    void getNeighborInfo();
    void passPotato();
};

#endif