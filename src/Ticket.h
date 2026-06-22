#ifndef TICKET_H
#define TICKET_H

#include <string>
#include <iostream>
#include <iomanip>

class Ticket {
protected:
    int id;
    std::string eventName;
    std::string buyerName;
    std::string seatNumber;
    double basePrice;
    bool isBooked;

public:
    Ticket(int id, const std::string& eventName, const std::string& seatNumber, double basePrice)
        : id(id), eventName(eventName), buyerName(""), seatNumber(seatNumber), basePrice(basePrice), isBooked(false) {}

    virtual ~Ticket() = default;

    // Getters and Setters
    int getId() const { return id; }
    std::string getEventName() const { return eventName; }
    std::string getBuyerName() const { return buyerName; }
    std::string getSeatNumber() const { return seatNumber; }
    double getBasePrice() const { return basePrice; }
    bool getIsBooked() const { return isBooked; }

    void book(const std::string& buyer) {
        buyerName = buyer;
        isBooked = true;
    }

    void cancel() {
        buyerName = "";
        isBooked = false;
    }

    // Pure virtual functions for polymorphism
    virtual double calculatePrice() const = 0;
    virtual std::string getTicketType() const = 0;

    // Virtual print function
    virtual void displayInfo() const {
        std::cout << "------------------------------------------\n";
        std::cout << " [票券 ID]   : " << id << "\n";
        std::cout << " [票券類型] : " << getTicketType() << "\n";
        std::cout << " [活動名稱] : " << eventName << "\n";
        std::cout << " [座位號碼] : " << seatNumber << "\n";
        std::cout << " [票價金額] : $" << std::fixed << std::setprecision(0) << calculatePrice() << " TWD\n";
        std::cout << " [購買狀態] : " << (isBooked ? "已預約 (購買者: " + buyerName + ")" : "空閒 (未售出)") << "\n";
    }
};

#endif // TICKET_H
