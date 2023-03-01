#ifndef POTATO_H
#define POTATO_H

#include <iostream>
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
    Potato() : hops(0), count(0) {
        // initialize playerID to 0
        memset(playerID, 0, sizeof(playerID));
    }
    Potato(int hops) : hops(hops), count(0) {
        // initialize playerID to 0
        memset(playerID, 0, sizeof(playerID));
    }
};

#endif
