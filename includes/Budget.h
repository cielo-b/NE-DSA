#ifndef BUDGET_H
#define BUDGET_H

#include <string>

class Budget
{
    std::string id;
    double amount;
    Budget() : id(""), amount(0.0) {}
    Budget(const std::string &id, double amount) : id(id), amount(amount) {}

    bool create(const std::string &id, const double &amount);
};
#endif