#ifndef EVENT_H
#define EVENT_H

#include <string>

struct Event {
    std::string name;
    double basePrice;
    int rows;
    int cols;

    Event(const std::string& name, double basePrice, int rows, int cols)
        : name(name), basePrice(basePrice), rows(rows), cols(cols) {}
};

#endif // EVENT_H
