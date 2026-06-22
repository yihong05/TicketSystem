#ifndef REGULAR_TICKET_H
#define REGULAR_TICKET_H

#include "Ticket.h"

class RegularTicket : public Ticket {
public:
    RegularTicket(int id, const std::string& eventName, const std::string& seatNumber, double basePrice)
        : Ticket(id, eventName, seatNumber, basePrice) {}

    double calculatePrice() const override {
        return basePrice;
    }

    std::string getTicketType() const override {
        return "一般票 (Regular)";
    }

    void displayInfo() const override {
        Ticket::displayInfo();
        std::cout << "------------------------------------------\n";
    }
};

#endif // REGULAR_TICKET_H
