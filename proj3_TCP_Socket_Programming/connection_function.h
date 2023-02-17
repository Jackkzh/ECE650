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
#include "potato.h"

using namespace std;

int serverStartConnection(const char *port);
int clientInit(const char *host, const char *port);

vector<int> clientJoinConnection(const char *port, vector<int> player_fd, int player_num, int socket_fd);

// client establish connection with each other
