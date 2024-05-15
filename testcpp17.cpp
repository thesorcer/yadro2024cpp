#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <iomanip>

using namespace std;

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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    int num_tables;
    string start_time, end_time;
    int cost_per_hour;

    input_file >> num_tables >> start_time >> end_time >> cost_per_hour;
    input_file.ignore(); // Ignore the newline character after the cost per hour.

    int start_minutes = timeToMinutes(start_time);
    int end_minutes = timeToMinutes(end_time);

    vector<Table> tables(num_tables);
    for (int i = 0; i < num_tables; ++i) {
        tables[i] = { i + 1, "", false, 0, 0 };
    }

    unordered_map<string, Client> clients;
    queue<string> waiting_queue;

    string line;
    vector<string> output_events;
    output_events.push_back(start_time);

    while (getline(input_file, line)) {
        vector<string> tokens = split(line, ' ');

        if (tokens.size() < 3) {
            cerr << "Invalid input format: " << line << endl;
            return 1;
        }

        string event_time = tokens[0];
        int event_minutes = timeToMinutes(event_time);
        int event_id = stoi(tokens[1]);
        string client_name = tokens[2];

        if (event_minutes < start_minutes || event_minutes >= end_minutes) {
            output_events.push_back(line);
            output_events.push_back(event_time + " 13 NotOpenYet");
            continue;
        }

        if (event_id == 1) { // Client arrives
            if (clients.count(client_name)) {
                output_events.push_back(line);
                output_events.push_back(event_time + " 13 YouShallNotPass");
            }
            else {
                clients[client_name] = { client_name, 0, event_time, false };
                output_events.push_back(line);
            }
        }
        else if (event_id == 2) { // Client sits at a table
            int table_number = stoi(tokens[3]);
            if (clients.find(client_name) == clients.end()) {
                output_events.push_back(line);
                output_events.push_back(event_time + " 13 ClientUnknown");
            }
            else {
                Client& client = clients[client_name];
                if (tables[table_number - 1].is_occupied) {
                    output_events.push_back(line);
                    output_events.push_back(event_time + " 13 PlaceIsBusy");
                }
                else {
                    if (client.table != 0) {
                        Table& old_table = tables[client.table - 1];
                        int occupied_minutes = event_minutes - timeToMinutes(old_table.occupied_since);
                        old_table.total_occupied_minutes += occupied_minutes;
                        old_table.total_earnings += ((occupied_minutes + 59) / 60) * cost_per_hour;
                        old_table.is_occupied = false;
                    }
                    client.table = table_number;
                    tables[table_number - 1].is_occupied = true;
                    tables[table_number - 1].occupied_since = event_time;
                    output_events.push_back(line);
                }
            }
        }
        else if (event_id == 3) { // Client waits
            if (clients.find(client_name) == clients.end()) {
                output_events.push_back(line);
                output_events.push_back(event_time + " 13 ClientUnknown");
            }
            else {
                bool has_free_table = false;
                for (const auto& table : tables) {
                    if (!table.is_occupied) {
                        has_free_table = true;
                        break;
                    }
                }
                if (has_free_table) {
                    output_events.push_back(line);
                    output_events.push_back(event_time + " 13 ICanWaitNoLonger!");
                }
                else {
                    waiting_queue.push(client_name);
                    clients[client_name].waiting = true;
                    output_events.push_back(line);
                }
            }
        }
        else if (event_id == 4) { // Client leaves
            if (clients.find(client_name) == clients.end()) {
                output_events.push_back(line);
                output_events.push_back(event_time + " 13 ClientUnknown");
            }
            else {
                Client& client = clients[client_name];
                if (client.table != 0) {
                    Table& table = tables[client.table - 1];
                    int occupied_minutes = event_minutes - timeToMinutes(table.occupied_since);
                    table.total_occupied_minutes += occupied_minutes;
                    table.total_earnings += ((occupied_minutes + 59) / 60) * cost_per_hour;
                    table.is_occupied = false;
                    client.table = 0;
                    output_events.push_back(line);
                    if (!waiting_queue.empty()) {
                        string next_client_name = waiting_queue.front();
                        waiting_queue.pop();
                        clients[next_client_name].waiting = false;
                        clients[next_client_name].table = table.number;
                        table.is_occupied = true;
                        table.occupied_since = event_time;
                        output_events.push_back(event_time + " 12 " + next_client_name + " " + to_string(table.number));
                    }
                }
                else {
                    output_events.push_back(line);
                }
                clients.erase(client_name);
            }
        }
        else {
            cerr << "Invalid event ID: " << event_id << " in line: " << line << endl;
            return 1;
        }
    }

    for (auto& [name, client] : clients) {
        if (client.table != 0) {
            Table& table = tables[client.table - 1];
            int occupied_minutes = end_minutes - timeToMinutes(table.occupied_since);
            table.total_occupied_minutes += occupied_minutes;
            table.total_earnings += ((occupied_minutes + 59) / 60) * cost_per_hour;
            output_events.push_back(end_time + " 11 " + name);
        }
    }

    output_events.push_back(end_time);

    for (const auto& table : tables) {
        output_events.push_back(to_string(table.number) + " " + to_string(table.total_earnings) + " " +
            minutesToTime(table.total_occupied_minutes));
    }

    for (const auto& event : output_events) {
        cout << event << endl;
    }

    return 0;
}
