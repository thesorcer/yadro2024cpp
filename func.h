#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <queue>

// Чтение файла с данными построчно
void readFile(std::vector<std::string>& input, const std::string& fileName);

// Перевод строки, содержащей время XX:XX в число минут
int timeToMinutes(const std::string&);

// Перевод числа минут в строку с временем в формате XX:XX
std::string minutesToTime(int);


void readFile(std::vector<std::string>& input, const std::string &fileName)
{
    std::ifstream in;
    in.open(fileName);

    std::string str;
    while (!in.eof())
    {
        str = " ";
        std::getline(in, str);
        input.push_back(str);
    }
}

int timeToMinutes(const std::string& timeStr) {
    int hours, minutes;
    char colon;

    std::stringstream ss(timeStr);
    ss >> hours >> colon >> minutes;

    return hours * 60 + minutes;
}

std::string minutesToTime(int timeInt) {
    int hours = timeInt / 60;
    int minutes = timeInt % 60;

    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes;

    return ss.str();
}
