//include necessary header files
#include <iostream>
// include header for memset
#include <cstring>

class Potato {
public:
    int hops;
    int count;
    int playerID[512];

    // write a constructor using initialization list, set everything in playerID to 0 using memset
    Potato(int hops) : hops(hops), count(0) {
        memset(playerID, 0, sizeof(playerID));
    }
};

