#ifndef CONNECTION_FUNCTION_H
#define CONNECTION_FUNCTION_H
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <vector>
// add header for NULL
#include <stdio.h>
// add header for memset
#include <string.h>
// add header for exit
#include <stdlib.h>
#include <iostream>

using namespace std;

int initServer(const char *port);
int clientInit(const char *host, const char *port);

void clientJoinConnection(int socket_fd, vector<int> &player_fd_list,
                                 vector<int> &player_port_list, vector<char *> &player_ip_list, int player_num);

// players establish connection with each other
void playerConnect(vector<int> &player_fd_list, vector<int> &player_port_list, vector<char *> &player_ip_list);

// void playerRingConnect(char *client_host, int &client_port, int &server_fd, int player_fd, char * my_port);
char * convertPort(int port);


#endif