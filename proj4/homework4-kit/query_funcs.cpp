#include "query_funcs.h"

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg) {
    stringstream ss;
    work W(*C);
    ss << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES ("
       << team_id << ", " << jersey_num << ", " << W.quote(first_name) << ", " << W.quote(last_name) << ", "
       << mpg << ", " << ppg << ", " << rpg << ", " << apg << ", " << spg << ", " << bpg << ")";
    W.exec(ss.str());
    W.commit();

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
    // W.exec("INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES (" +
    //        to_string(team_id) + ", " + to_string(jersey_num) + ", '" + sanitized_first_name + "', '" + sanitized_last_name + "', " +
    //        to_string(mpg) + ", " + to_string(ppg) + ", " + to_string(rpg) + ", " + to_string(apg) + ", " +
    //        to_string(spg) + ", " + to_string(bpg) + ")");
    //        //+ team_id  and quote 
    // W.commit();

}

void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses) {
    work W(*C);
    stringstream ss;
    ss << "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("
       << W.quote(name) << ", " << state_id << ", " << color_id << ", " << wins << ", " << losses << ")";
    W.exec(ss.str());
    // W.exec("INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ('" +
    //        name + "', " + to_string(state_id) + ", " + to_string(color_id) +
    //        ", " + to_string(wins) + ", " + to_string(losses) + ")");
    W.commit();
}

void add_state(connection *C, string name) {
    work W(*C);
    stringstream ss;
    ss << "INSERT INTO STATE (NAME) VALUES (" << W.quote(name) << ")";
    // W.exec("INSERT INTO STATE VALUES (DEFAULT, '" + name + "');");
    W.exec(ss.str());
    W.commit();
}

void add_color(connection *C, string name) {
    work W(*C);
    stringstream ss;
    ss << "INSERT INTO COLOR (NAME) VALUES (" << W.quote(name) << ")";
    W.exec(ss.str());
    //W.exec("INSERT INTO COLOR (NAME) VALUES ('" + name + "');");
    W.commit();
}

/*
 * Query 1 - Find the player within the given search attributes ranges 
 * All use_ params are used as flags for corresponding attributes
 * 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * 0 for a use_ param means this attribute is disabled
 */
void query1(connection *C,
            int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg) {
    // select * form table PLAYER, if use_mpg is 1, then add where clause
    // if use_mpg is 0, then don't add where clause
    stringstream ss;
    ss << "SELECT * FROM PLAYER";
    bool isFirst = true;
    if (use_mpg) {
        ss << " WHERE (MPG BETWEEN " << min_mpg << " AND " << max_mpg << ")";
        isFirst = false;
    }
    if (use_ppg) {
        if (isFirst) {
            ss << " WHERE (PPG BETWEEN " << min_ppg << " AND " << max_ppg << ")";
            isFirst = false;
        } else {
            ss << " AND (PPG BETWEEN " << min_ppg << " AND " << max_ppg << ")";
        }
    }
    if (use_rpg) {
        if (isFirst) {
            ss << " WHERE (RPG BETWEEN " << min_rpg << " AND " << max_rpg << ")";
            isFirst = false;
        } else {
            ss << " AND (RPG BETWEEN " << min_rpg << " AND " << max_rpg << ")";
        }
    }
    if (use_apg) {
        if (isFirst) {
            ss << " WHERE (APG BETWEEN " << min_apg << " AND " << max_apg << ")";
            isFirst = false;
        } else {
            ss << " AND (APG BETWEEN " << min_apg << " AND " << max_apg << ")";
        }
    }
    if (use_spg) {
        if (isFirst) {
            ss << " WHERE (SPG BETWEEN " << min_spg << " AND " << max_spg << ")";
            isFirst = false;
        } else {
            ss << " AND (SPG BETWEEN " << min_spg << " AND " << max_spg << ")";
        }
    }
    if (use_bpg) {
        if (isFirst) {
            ss << " WHERE (BPG BETWEEN " << min_bpg << " AND " << max_bpg << ")";
            isFirst = false;
        } else {
            ss << " AND (BPG BETWEEN " << min_bpg << " AND " << max_bpg << ")";
        }
    }
    ss << ";";

    nontransaction N(*C);
    result R(N.exec(ss.str()));
    // print the result of the query
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << endl;
    // set precision to 1 , so 0 will be printed as 0.0
    cout << setprecision(1) << fixed;
    
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>() << " " << c[3].as<string>() << " " << c[4].as<string>() << " " << c[5].as<int>() << " " << c[6].as<int>() << " " << c[7].as<int>() << " " << c[8].as<int>() << " " << c[9].as<double>() << " " << c[10].as<double>() << endl;
    }
}

/**
 * Query 2 - Find the names of all teams with a certain color
 * @param C - connection to the database
 * @param team_color - color of the team
*/
void query2(connection *C, string team_color) {
    stringstream ss;
    nontransaction N(*C);
    cout << "NAME" << endl;
    ss << "SELECT TEAM.NAME FROM TEAM, COLOR WHERE TEAM.COLOR_ID = COLOR.COLOR_ID AND COLOR.NAME = " << N.quote(team_color) << ";";
    
    result R(N.exec(ss.str()));

    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << endl;
    }
}

/**
 * Query 3 - Find the Fisrt Names, Last Names of all players on a certain team
 * @param C - connection to the database
 * @param team_name - name of the team
*/
void query3(connection *C, string team_name) {
    stringstream ss;
    nontransaction N(*C);
    cout << "FIRST_NAME LAST_NAME" << endl;
    ss << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.NAME = " << N.quote(team_name) << " ORDER BY PPG DESC;";
    result R(N.exec(ss.str()));
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << endl;
    }
}

/**
 * Query 4 - Find the Uniform numbers, Fisrt names, Last names of all players on a certain team with a certain color
 * @param C - connection to the database
 * @param team_state - state of the team
*/
void query4(connection *C, string team_state, string team_color) {
    stringstream ss;
    nontransaction N(*C);
    
    ss << "SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM, COLOR, STATE WHERE TEAM.STATE_ID = STATE.STATE_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID AND PLAYER.TEAM_ID = TEAM.TEAM_ID AND STATE.NAME = " << N.quote(team_state) << " AND COLOR.NAME = " << N.quote(team_color) << ";";
    result R(N.exec(ss.str()));
    cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<int>() << " " << c[1].as<string>() << " " << c[2].as<string>() << endl;
    }
}

/**
 * Query 5 - Find the First names, Last names, Team names, and number of wins for 
 * all teams with more than a certain number of wins
 * @param C - connection to the database
 * @param num_wins - number of wins
*/
void query5(connection *C, int num_wins) {
    stringstream ss;
    nontransaction N(*C);
    cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
    ss << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, TEAM.WINS FROM PLAYER, TEAM WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.WINS > " << num_wins << ";";
    result R(N.exec(ss.str()));
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << " " << c[2].as<string>() << " " << c[3].as<int>() << endl;
    }
}
