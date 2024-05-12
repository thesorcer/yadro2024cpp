#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <queue>

#include "Functions.h";

class Table
{
    friend class Club;

private:
    int number;
    bool isFree = true;
    int money = 0;
    int workTime = 0;

public:
    Table(const int number) : number{ number }
    {}
};

class Client
{
    friend class Club;

private:
    int sitTime = 0;
    int leavingTime = 0;
    int id;
    std::string eventTime;
    std::string name;
    int tableNumber = 0;

public:
    Client(const std::string comingTime, const int id, const std::string name, const int tableNumber)
    {
        this->eventTime = comingTime;
        this->id = id;
        this->name = name;
        this->tableNumber = tableNumber;

    }

    std::string getTime() const { return eventTime; }
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getTable() const { return tableNumber; }

    bool operator == (const Client& client) const
    {
        return (client.name == this->name);
    }
};

std::ostream& operator << (std::ostream& os, const Client& client)
{
    return os << client.getTime() << " " << client.getId() << " " << client.getName();
}

class Club {

private:
    int openTime;
    int closeTime;
    int allTables;
    int price;

    std::vector<Client> clients;
    std::queue<Client> waiting;
    std::vector<Table> tables;

public:
    Club(std::vector<std::string>& input)
    {

        std::istringstream iss(input[1]);
        std::string openTime, closeTime;
        iss >> openTime >> closeTime;

        this->openTime = timeToMinutes(openTime);
        this->closeTime = timeToMinutes(closeTime);
        this->allTables = stoi(input[0]);
        this->price = stoi(input[2]);

        for (int i = 0; i < allTables; i++)
        {
            tables.push_back(i + 1);
        }
    }

    void handleEvent(Client& client);
    void endOfDay();

    bool clubIsOpen(const int enterTime) const;
    bool isInClub(const Client& client) const;
    bool isTableFree(const int tableNumber) const;
    bool isAllTablesFree() const;
};

int main(int argc, char* argv[])
{
    std::vector<std::string> input;
    readFile(input, "input3.txt");

    std::vector<Client> clients;

    // Проход по массиву с 3 элемента, т.к. 0, 1 и 2 строки это столы, время работы клуба и стоимость. Дальше идут клиенты
    for (int i = 3; i < input.size(); i++)
    {
        std::istringstream iss(input[i]);
        std::string comingTime;
        int id;
        std::string name;
        int tableNumber;

        iss >> comingTime >> id >> name >> tableNumber;
        int timeInt = timeToMinutes(comingTime);
        if (timeInt < 0 || timeInt > 1439) break;

        Client cl(comingTime, id, name, tableNumber);
        clients.push_back(cl);
    }

    Club club(input);

    for (int i = 0; i < 3; i++)
    {
        std::cout << input[i] << std::endl;
    }
    for (auto& cl : clients)
    {
        club.handleEvent(cl);
    }

    club.endOfDay();

}

void Club::handleEvent(Client& client)
{

    int timeInt = timeToMinutes(client.eventTime);

    switch (client.id)
    {
    case 1:
        std::cout << client << std::endl;
        if (!clubIsOpen(timeInt))
        {
            std::cout << client.eventTime << " 13 NotOpenYet\n";
            return;
        }
        if (isInClub(client))
        {
            std::cout << client.eventTime << " 13 YouShallNotPass\n";
            return;
        }
        else
        {
            if (clients.size() < tables.size())
                clients.push_back(client);
            else
            {
                waiting.push(client);
                if (waiting.size() > tables.size())
                {
                    waiting.pop();
                    std::cout << client.eventTime << " 11 " << client.name << std::endl;
                    return;
                }
            }
            return;
        }
        break;

    case 2:
        std::cout << client << " " << client.tableNumber << std::endl;
        if (!isTableFree(client.tableNumber))
        {
            std::cout << client.eventTime << " 13 PlaceIsBusy\n";
            return;
        }
        if (!isInClub(client))
        {
            std::cout << client.eventTime << " 13 ClientUnknown\n";
            return;
        }
        else
        {
            tables[client.tableNumber - 1].isFree = false;
            for (auto& cl : clients)
            {
                if (client == cl)
                {
                    cl.tableNumber = client.tableNumber;
                    cl.sitTime = timeInt;
                }
            }
            tables[client.tableNumber - 1].money += price;

            return;
        }
        break;

    case 3: // В кейсе 3 добавлено условие, есть ли клиент в клубе. Потому что, если его нет, то почему он тогда ждет? 
        std::cout << client << std::endl;
        if (!isInClub(client))
        {
            std::cout << client.eventTime << " 13 ClientUnknown\n";
            return;
        }
        if (isAllTablesFree())
        {
            std::cout << client.eventTime << " 13 ICanWaitNoLonger!\n";
            return;
        }
        break;

    case 4:
        std::cout << client << std::endl;
        if (!isInClub(client))
        {
            std::cout << client.eventTime << " 13 ClientUnknown\n";
            return;
        }
        else
        {
            if (!waiting.empty()) {
                for (int i = 0; i < clients.size(); i++)
                {
                    if (clients[i] == client)
                    {
                        clients[i] = waiting.front();
                        waiting.pop();
                        tables[i].money += price;
                        std::cout << client.eventTime << " 12 " << clients[i].name << " " << tables[i].number << std::endl;
                        return;
                    }
                }
            }
            else
            {
                for (int i = 0; i < clients.size(); i++)
                {
                    if (client == clients[i])
                    {
                        clients[i].leavingTime = timeInt;
                        tables[client.tableNumber - 1].money += (clients[i].leavingTime - clients[i].sitTime + 60 - 1) / 60 * price;
                        clients.erase(clients.begin() + i);
                        tables[i].isFree = true;
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

bool Club::clubIsOpen(int enterTime) const
{
    return (enterTime >= openTime && enterTime <= closeTime);
}

bool Club::isInClub(const Client& client) const
{
    for (auto& cl : clients)
    {
        if (cl == client)
            return true;
    }
    return false;
}

bool Club::isTableFree(const int tableNumber) const
{
    return (tables[tableNumber - 1].isFree);
}

bool Club::isAllTablesFree() const
{
    for (auto& tb : tables)
    {
        if (tb.isFree == false)
            return false;
    }
    return true;
}

void Club::endOfDay()
{

    for (int i = 0; i < clients.size(); )
    {
        std::cout << minutesToTime(closeTime) << " 11 " << clients[i].name << std::endl;
        clients[i].leavingTime = closeTime;
        tables[clients[i].tableNumber - 1].workTime = clients[i].leavingTime - clients[i].sitTime;
        clients.erase(clients.begin() + i);
    }

    std::cout << minutesToTime(closeTime) << std::endl;

    for (auto& tb : tables)
    {

        std::cout << std::to_string(tb.number) << " " << std::to_string(tb.money) << " " << minutesToTime(tb.workTime) << std::endl;
    }

}

