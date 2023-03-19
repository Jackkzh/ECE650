#ifndef _HELPER_
#define _HELPER_

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <sstream>
#include <fstream>


using namespace std;
using namespace pqxx;

void dropExistingTable(connection *C, string table_name);
void createTable(connection *C, const char * sql_file);
void insertState(connection *C, const char *state_file);
void insertColor(connection *C, const char *color_file);
void insertTeam(connection *C, const char *team_file);
void insertPlayer(connection *C, const char *player_file);


void printTable(connection *C, string table_name);
void printColor(connection* C, string table_name);
void printPlayer(connection* C, string table_name);
void printTeam(connection* C, string table_name);
#endif