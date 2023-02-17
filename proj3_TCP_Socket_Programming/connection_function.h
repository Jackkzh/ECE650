#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <vector>
// add header for NULL
#include <stdio.h>
// add header for memset
#include <string.h>
// add header for exit
#include <stdlib.h>

#include <iostream>

using namespace std;

int serverStartConnection(const char *port);
int clientInit(const char *host, const char *port);

void clientJoinConnection(int socket_fd, vector<int> &player_fd_list,
                                 vector<int> &player_port_list, vector<char *> &player_ip_list, int player_num);

void playerConnect(vector<int> &player_fd_list, vector<int> &player_port_list, vector<char *> &player_ip_list);
// client establish connection with each other
