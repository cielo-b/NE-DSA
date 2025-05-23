#ifndef ROAD_H
#define ROAD_H

#include <iostream>
#include <vector>
#include <string.h>
using namespace std;

class Road
{
private:
    string id;
    string cityId1;
    string cityId2;

public:
    Road() : id(""), cityId1(""), cityId2("") {}
    Road(const string &id, const string &cityId1, const string &cityId2) : id(id), cityId1(cityId1), cityId2(cityId2) {}

    bool exists(const string &id);
    bool create(const string &id, const string &cityId1, const string &cityId2);
};

#endif