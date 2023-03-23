#include "helper.h"
#include "query_funcs.h"

/**
 * Drop a table if it exists
 * @param C the connection to the database
 * @param table_name the name of the table to drop
 */
void dropExistingTable(connection *C, string table_name) {
    /* Create a transactional object. */
    const string db_name = C->dbname();
    work W(*C);
    /* Execute SQL query */
    W.exec("DROP TABLE IF EXISTS " + table_name + ";");
    W.commit();
}

// create table for the database, read from the .sql file(sql sentences are stored in it) and execute the sql query
void createTable(connection *C, const char *sql_file) {
    try {
        ifstream file(sql_file);
        if (!file.is_open()) {
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            throw std::ifstream::failure("Error opening file");
        }
        stringstream buffer;
        buffer << file.rdbuf();
        string sql = buffer.str();
        // cout << sql << endl;
        file.close();
        work W(*C);
        /* Execute SQL query */
        W.exec(sql);
        W.commit();
    } catch (ifstream::failure &e) {
        cerr << e.what() << std::endl;
        // cout << "Error: cannot open file" << endl;
    }
}

// read from state.txt and insert into STATE table
void insertState(connection *C, const char *state_file) {
    try {
        ifstream file(state_file);
        if (!file.is_open()) {
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            throw std::ifstream::failure("Error opening file");
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string abbreviation = line.substr(line.find(" ") + 1);
            
            add_state(C, abbreviation);
            
            /*
            work W(*C);
            W.exec("INSERT INTO STATE VALUES (DEFAULT, '" + abbreviation + "');");
            W.commit();
            */
        }
        file.close();
    } catch (ifstream::failure &e) {
        cerr << e.what() << std::endl;
        // cout << "Error: cannot open file" << endl;
    }
}

// read from color.txt and insert into COLOR table
void insertColor(connection *C, const char *state_file) {
    try {
        ifstream file(state_file);
        if (!file.is_open()) {
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            throw std::ifstream::failure("Error opening file");
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, color;
            ss >> id >> color;
            //string color = line.substr(line.find(" ") + 1);
            add_color(C, color);
            /*
            work W(*C);
            W.exec("INSERT INTO COLOR (NAME) VALUES ('" + color + "');");
            W.commit();
            */
        }
        file.close();
    } catch (ifstream::failure &e) {
        cerr << e.what() << std::endl;
        // cout << "Error: cannot open file" << endl;
    }
}

// read from team.txt and insert into TEAM table
void insertTeam(connection *C, const char *team_file) {
    try {
        ifstream file(team_file);
        if (!file.is_open()) {
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            throw std::ifstream::failure("Error opening file");
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            // string color = line.substr(line.find(" ") + 1);
            int state_id, color_id, win, loss;
            string team_name, team_id;
            //ss.ignore(numeric_limits<streamsize>::max(), ' ');
            ss >> team_id >> team_name >> state_id >> color_id >> win >> loss;
            add_team(C, team_name, state_id, color_id, win, loss);
            /*
            work W(*C);
            W.exec("INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ('" +
                   team_name + "', " + to_string(state_id) + ", " + to_string(color_id) +
                   ", " + to_string(win) + ", " + to_string(loss) + ")");
            W.commit();
            */
        }
        file.close();
    } catch (ifstream::failure &e) {
        cerr << e.what() << std::endl;
        // cout << "Error: cannot open file" << endl;
    }
}

/**
 * read from player.txt and insert into PLAYER table
 * @param C the connection to the database
 */
void insertPlayer(connection *C, const char *player_file) {
    try {
        ifstream file(player_file);
        if (!file.is_open()) {
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            throw std::ifstream::failure("Error opening file");
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            // string color = line.substr(line.find(" ") + 1);
            int team_id, number, mpg, ppg, rpg, apg;
            float spg, bpg;
            string player_id, first_name, last_name;
            //ss.ignore(numeric_limits<streamsize>::max(), ' ');
            ss >> player_id >> team_id >> number >> first_name >> last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg;      

            add_player(C, team_id, number, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);

            // string sanitized_first_name = first_name;
            // size_t pos = first_name.find("'");
            // while (pos != string::npos) {
            //     sanitized_first_name.replace(pos, 1, "''");
            //     pos = sanitized_first_name.find("'", pos + 2);
            // }
            // string sanitized_last_name = last_name;
            // pos = last_name.find("'");
            // while (pos != string::npos) {
            //     sanitized_last_name.replace(pos, 1, "''");
            //     pos = sanitized_last_name.find("'", pos + 2);
            // }
            // work W(*C);
            // /* Execute SQL query */
            // W.exec("INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES (" +
            //        to_string(team_id) + ", " + to_string(number) + ", '" + sanitized_first_name + "', '" + sanitized_last_name + "', " +
            //        to_string(mpg) + ", " + to_string(ppg) + ", " + to_string(rpg) + ", " + to_string(apg) + ", " +
            //        to_string(spg) + ", " + to_string(bpg) + ")");
            // W.commit();
        }
        file.close();
    } catch (ifstream::failure &e) {
        cerr << e.what() << std::endl;
        // cout << "Error: cannot open file" << endl;
    }
}

/**
 * print out a table contents
 */
void printTable(connection *C, string table_name) {
    work W(*C);
    result r = W.exec("SELECT * FROM " + table_name + ";");
    cout << "STATE table: " << endl;
    // cout << "COLOR_ID " << " NAME " << endl;
    // cout << "STATE_ID " << " NAME " << endl;
    for (auto row : r) {
        std::cout << row["STATE_ID"].as<int>() << "\t" << row["NAME"].as<std::string>() << "\t" << std::endl;
    }
    W.commit();
}

// print out all the rows in the PLAYER table
void printPlayer(connection *C, string table_name) {
    work W(*C);
    result r = W.exec("SELECT * FROM " + table_name + ";");
    cout << "PLAYER table: " << endl;
    // cout << "COLOR_ID " << " NAME " << endl;
    // cout << "STATE_ID " << " NAME " << endl;
    for (auto row : r) {
        cout << row["PLAYER_ID"].as<int>() << "\t" << row["TEAM_ID"].as<int>() << "\t" << row["UNIFORM_NUM"].as<int>() << "\t" << row["FIRST_NAME"].as<string>() << "\t" << row["LAST_NAME"].as<string>() << "\t" << row["MPG"].as<int>() << "\t" << row["PPG"].as<int>() << "\t" << row["RPG"].as<int>() << "\t" << row["APG"].as<int>() << "\t" << row["SPG"].as<float>() << "\t" << row["BPG"].as<float>() << "\t" << std::endl;
        //cout << row["FIRST_NAME"].as<string>() << "\t" << row["LAST_NAME"].as<string>() << endl;
    }
    W.commit();
}

// print out all the rows in the TEAM table
void printTeam(connection *C, string table_name) {
    work W(*C);
    result r = W.exec("SELECT * FROM " + table_name + ";");
    cout << "TEAM table: " << endl;
    // cout << "COLOR_ID " << " NAME " << endl;
    // cout << "STATE_ID " << " NAME " << endl;
    for (auto row : r) {
        std::cout << row["TEAM_ID"].as<int>() << "\t" << row["NAME"].as<std::string>() << "\t" << row["STATE_ID"].as<int>() << "\t" << row["COLOR_ID"].as<int>() << "\t" << row["WINS"].as<int>() << "\t" << row["LOSSES"].as<int>() << "\t" << std::endl;
    }
    W.commit();
}

// print out all the rows in the COLOR table
void printColor(connection *C, string table_name) {
    work W(*C);
    result r = W.exec("SELECT * FROM " + table_name + ";");
    cout << "COLOR table: " << endl;
    // cout << "COLOR_ID " << " NAME " << endl;
    // cout << "STATE_ID " << " NAME " << endl;
    for (auto row : r) {
        std::cout << row["COLOR_ID"].as<int>() << "\t" << row["NAME"].as<std::string>() << "\t" << std::endl;
    }
    W.commit();
}