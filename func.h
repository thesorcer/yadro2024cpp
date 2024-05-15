#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <iomanip>

using namespace std;

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int timeToMinutes(const string& time) {
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}

string minutesToTime(int total_minutes) {
    int hours = total_minutes / 60;
    int minutes = total_minutes % 60;
    ostringstream oss;
    oss << setfill('0') << setw(2) << hours << ":" << setfill('0') << setw(2) << minutes;
    return oss.str();
}

struct Client {
    string name;
    int table;
    string arrival_time;
    bool waiting;
};

struct Table {
    int number;
    string occupied_since;
    bool is_occupied;
    int total_earnings;
    int total_occupied_minutes;
};
