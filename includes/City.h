#ifndef CITY_H
#define CITY_H

#include <iostream>
#include <vector>
#include <string.h>
using namespace std;

class City
{
private:
    string id;
    string name;

public:
    bool exists(const string &id);
    bool create(const string &id, const string &name);
    
    City():id(""), name("") {}
    City(const string &id, const string &name): id(id), name(name) {}
};

#endif