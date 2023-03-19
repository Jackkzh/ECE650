#include "query_funcs.h"

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg) {
}

void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses) {
}

void add_state(connection *C, string name) {
}

void add_color(connection *C, string name) {
}

/*
 * All use_ params are used as flags for corresponding attributes
 * a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * a 0 for a use_ param means this attribute is disabled
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
    cout << ss.str() << endl;
    // execute the query
    work W(*C);
    result R(W.exec(ss.str()));
    cout << "number of rows: " << R.size() << endl;
    // print the result of the query
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>() << " " << c[3].as<string>() << " " << c[4].as<string>() << " " << c[5].as<int>() << " " << c[6].as<int>() << " " << c[7].as<int>() << " " << c[8].as<int>() << " " << c[9].as<double>() << " " << c[10].as<double>() << endl;
    }
}

void query2(connection *C, string team_color) {
    stringstream ss;
    ss << "SELECT NAME FROM TEAM WHERE COLOR = '" << team_color << "';";
    work W(*C);
    result R(W.exec(ss.str()));
    
}

void query3(connection *C, string team_name) {
}

void query4(connection *C, string team_state, string team_color) {
}

void query5(connection *C, int num_wins) {
}
