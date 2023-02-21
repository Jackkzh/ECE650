#ifndef POTATO_H
#define POTATO_H

#include <iostream>
// include header for memset
#include <cstring>
#include <vector>

using namespace std;

class Potato {
   public:
    int hops;
    int count;
    int playerID[512];

    // write a constructor using initialization list, set everything in playerID
    // to 0 using memset
    Potato() : hops(-1), count(0) {
        memset(playerID, 0, sizeof(playerID));
        // initialize playerID to 0
    }
    Potato(int hops) : hops(hops), count(0) {
        // initia
        memset(playerID, 0, sizeof(playerID));
        // initialize playerID to 0
    }
};

#endif
