#include <iostream>
#include <pqxx/pqxx>

#include "exerciser.h"
#include "helper.h"
#include "query_funcs.h"

using namespace std;
using namespace pqxx;

int main(int argc, char *argv[]) {
    // Allocate & initialize a Postgres connection object
    connection *C;

    try {
        // Establish a connection to the database
        // Parameters: database name, user name, user password
        C = new connection("dbname=acc_bball user=postgres password=passw0rd");
        if (C->is_open()) {
            cout << "Opened database successfully: " << C->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return 1;
    }

    // TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
    //       load each table with rows from the provided source txt files

    // check if tables exist, if so, drop them; then create tables
    dropExistingTable(C, "PLAYER");
    dropExistingTable(C, "TEAM");
    dropExistingTable(C, "STATE");
    dropExistingTable(C, "COLOR");
    createTable(C, "tables.sql");

    insertState(C, "state.txt");
    insertColor(C, "color.txt");
    insertTeam(C, "team.txt");
    insertPlayer(C, "player.txt");
    
    //cout << "*********Insert complete********" << endl;

    // printTable(C, "STATE");
    // printColor(C, "COLOR");
    // printTeam(C, "TEAM");
    // printPlayer(C, "PLAYER");

    exercise(C);
    C->disconnect();

    return EXIT_SUCCESS;
}
