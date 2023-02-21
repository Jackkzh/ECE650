#include <unistd.h>

#include "connection_function.h"
#include "potato.h"

int main(int argc, char *argv[]) {
    // check for the number of arguments steps
    if (argc != 4) {
        cerr << "Error: invalid number of arguments" << endl;
        return EXIT_FAILURE;
    }

    // get the port number
    const char *port = argv[1];
    int player_num = atoi(argv[2]);
    int hop_num = atoi(argv[3]);

    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << player_num << endl;
    cout << "Hops = " << hop_num << endl;

    // server inits socket and listen to the connection
    int socket_fd = serverStartConnection(port);

    // accept the connection from all the players
    vector<int> player_fd_list;
    vector<int> player_port_list;
    vector<char *> player_ip_list;
    clientJoinConnection(socket_fd, player_fd_list, player_port_list, player_ip_list, player_num);
    cout << "All players present, ready to play" << endl;
    // print out all the port numbers and ip addresses of the players
    for (int i = 0; i < player_num; i++) {
        cout << "Player " << i << " has IP address " << player_ip_list[i] << " and port number " << player_port_list[i] << endl;
    }

    // for (int i = 0; i < player_num; i++) {
    //     int neighbor_id = (i + 1) % player_num;
    //     int neighbor_port = player_port_list[neighbor_id];
    //     char neighbor_ip[100];
    //     memset(neighbor_ip, 0, sizeof(neighbor_ip));
    //     strcpy(neighbor_ip, player_ip_list[neighbor_id]);
    //     send(player_fd_list[i], &neighbor_port, sizeof(neighbor_port), 0);
    //     send(player_fd_list[i], &neighbor_ip, sizeof(neighbor_ip), 0);
    // }

    playerConnect(player_fd_list, player_port_list, player_ip_list);

    // print all players' fd
    // for (int i = 0; i < player_num; i++) {
    //     cout << "player " << i << " fd: " << player_fd_list[i] << endl;
    // }

    // now creates a potato and send it to a random player

    // print our fdlist
    for (int i = 0; i < player_num; i++) {
        cout << "player " << i << " fd: " << player_fd_list[i] << endl;
    }

      //begin playing
  Potato potato(hop_num);


  if (potato.hops != 0) {  //send potato to first player
    srand((unsigned int)time(NULL) + player_num);
    int random = rand() % player_num;
    send(player_fd_list[random], &potato, sizeof(potato), 0);
    cout << "Ready to start the game, sending potato to player " << random << endl;

//     //receive last potato
//     fd_set readfds;
//     //int nfds = *max_element(player_fd_list.begin(), player_fd_list.end());
//     FD_ZERO(&readfds);
//     for (int i = 0; i < player_num; i++) {
//       FD_SET(player_fd_list[i], &readfds);
//     }
//     select(100, &readfds, NULL, NULL, NULL);
//     for (int i = 0; i < player_num; i++) {
//       if (FD_ISSET(player_fd_list[i], &readfds)) {
//         recv(player_fd_list[i], &potato, sizeof(potato), MSG_WAITALL);
//         break;
//       }
//     }
   }

//   //send potato with num_hops 0 to all players to shut down
//   for (int i = 0; i < player_num; i++) {
//     send(player_fd_list[i], &potato, sizeof(potato), 0);
//   }
//   cout << "Trace of potato:" << endl;
//   for (int i = 0; i < potato.count; i++) {
//     cout << potato.playerID[i];
//     if (i != potato.count - 1) {
//       cout << ",";
//     }
//     else {
//       cout << endl;
//     }
//   }

//   for (int i = 0; i < player_num; i++) {
//     close(player_fd_list[i]);
//   }
//   close(socket_fd);
  return 0;
}