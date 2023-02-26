#ifndef RINGMASTER_H
#define RINGMASTER_H

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "potato.h"

using namespace std;

class RingMaster {
   public:
    string hostname;
    string port;
    int hop_num;
    int listen_fd;
    int player_num;
    vector<int> player_fd_list;
    vector<int> player_port_list;
    vector<char *> player_ip_list;

    RingMaster() : hop_num(-1), listen_fd(-2), player_num(-1){};
    RingMaster(string _port, int _player_num, int _hop) : port(_port),
                                                          hop_num(_hop),
                                                          listen_fd(-2),
                                                          player_num(_player_num){};

    void initServer();
    void clientJoinConnection();
    void playerConnect();
    void playGame();
    void printTrace(Potato &potato);
};

#endif