#ifndef RINGMASTER_H
#define RINGMASTER_H

#include "connection_function.h"

using namespace std;

class RingMaster {
   public:

    string hostname;
    string port;
    int listen_fd;
    int player_num;
    vector<int> player_fd_list;
    vector<int> player_port_list;
    vector<char *> player_ip_list;

    RingMaster() : listen_fd(-2) {};
    RingMaster(string _host, string _port, int _player_num) : hostname(_host), port(_port), player_num(_player_num), listen_fd(-2), server_fd(-2) {};

    void initServer(const char *port);
    void clientJoinConnection();
    void playerConnect();
    //char *convertPort(int port);
};

#endif