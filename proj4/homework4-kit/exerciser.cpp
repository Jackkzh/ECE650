#include "exerciser.h"

void exercise(connection *C) {
    query1(C,
           1, 20, 39,
           1, 8, 16,
           0, 7, 10,
           1, 5, 10,
           0, 0, 0,
           0, 0, 0);


    query2(C, "Green");
    query3(C, "Duke");
    query4(C, "NC", "LightBlue");
    query5(C, 10);


}
