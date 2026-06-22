#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Event.h"
#include "Ticket.h"
#include "RegularTicket.h"
#include "VIPTicket.h"
#include "StudentTicket.h"

class DataManager {
private:
    std::string eventsFilePath;
    std::string bookingsFilePath;
    std::vector<Event> events;
    std::vector<std::shared_ptr<Ticket>> tickets;

    // Helper to trim whitespaces and newlines
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    DataManager(const std::string& eventsPath = "data/events.txt", 
                const std::string& bookingsPath = "data/bookings.txt")
        : eventsFilePath(eventsPath), bookingsFilePath(bookingsPath) {}

    const std::vector<Event>& getEvents() const { return events; }
    std::vector<std::shared_ptr<Ticket>>& getTickets() { return tickets; }

    // Load events from file and pre-populate all seats as unsold Regular tickets
    bool loadEvents() {
        events.clear();
        tickets.clear();

        std::ifstream file(eventsFilePath);
        if (!file.is_open()) {
            std::cerr << "[錯誤] 無法開啟活動資料檔: " << eventsFilePath << "\n";
            return false;
        }

        std::string line;
        int eventIndex = 0;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string name, priceStr, rowsStr, colsStr;

            if (std::getline(ss, name, ',') &&
                std::getline(ss, priceStr, ',') &&
                std::getline(ss, rowsStr, ',') &&
                std::getline(ss, colsStr, ',')) {

                name = trim(name);
                double basePrice = std::stod(trim(priceStr));
                int rows = std::stoi(trim(rowsStr));
                int cols = std::stoi(trim(colsStr));

                events.push_back(Event(name, basePrice, rows, cols));

                // Generate ticket slots for this event grid
                // Unique ticket ID: (eventIndex + 1) * 1000 + (row * 10) + (col + 1)
                for (int r = 0; r < rows; ++r) {
                    for (int c = 0; c < cols; ++c) {
                        int ticketId = (eventIndex + 1) * 1000 + (r * 10) + (c + 1);
                        std::string seatNum = "";
                        seatNum += (char)('A' + r);
                        seatNum += std::to_string(c + 1);

                        // Initially create as RegularTicket (unsold)
                        tickets.push_back(std::make_shared<RegularTicket>(ticketId, name, seatNum, basePrice));
                    }
                }
                eventIndex++;
            }
        }
        file.close();
        return true;
    }

    // Load existing bookings and reconstruct polymorphic Ticket subclasses (Regular, VIP, Student)
    bool loadBookings() {
        std::ifstream file(bookingsFilePath);
        if (!file.is_open()) {
            // It is okay if bookings.txt doesn't exist yet (first-time run)
            return true;
        }

        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string idStr, eventName, seatNum, typeStr, buyerName, priceStr, extraField;

            if (std::getline(ss, idStr, ',') &&
                std::getline(ss, eventName, ',') &&
                std::getline(ss, seatNum, ',') &&
                std::getline(ss, typeStr, ',') &&
                std::getline(ss, buyerName, ',') &&
                std::getline(ss, priceStr, ',') &&
                std::getline(ss, extraField, ',')) {

                int ticketId = std::stoi(trim(idStr));
                eventName = trim(eventName);
                seatNum = trim(seatNum);
                typeStr = trim(typeStr);
                buyerName = trim(buyerName);
                extraField = trim(extraField);

                // Find matching pre-populated ticket slot
                auto it = std::find_if(tickets.begin(), tickets.end(), [ticketId](const std::shared_ptr<Ticket>& t) {
                    return t->getId() == ticketId;
                });

                if (it != tickets.end()) {
                    double basePrice = (*it)->getBasePrice();
                    std::shared_ptr<Ticket> loadedTicket = nullptr;

                    if (typeStr == "VIP") {
                        loadedTicket = std::make_shared<VIPTicket>(ticketId, eventName, seatNum, basePrice, extraField);
                    } else if (typeStr == "Student") {
                        loadedTicket = std::make_shared<StudentTicket>(ticketId, eventName, seatNum, basePrice, extraField);
                    } else {
                        loadedTicket = std::make_shared<RegularTicket>(ticketId, eventName, seatNum, basePrice);
                    }

                    if (loadedTicket) {
                        loadedTicket->book(buyerName);
                        // Swap with our new specialized object
                        *it = loadedTicket;
                    }
                }
            }
        }
        file.close();
        return true;
    }

    // Save all currently booked tickets back to data/bookings.txt
    bool saveBookings() const {
        std::ofstream file(bookingsFilePath);
        if (!file.is_open()) {
            std::cerr << "[錯誤] 無法寫入訂單資料檔: " << bookingsFilePath << "\n";
            return false;
        }

        for (const auto& ticket : tickets) {
            if (ticket->getIsBooked()) {
                std::string typeStr = "Regular";
                std::string extraField = "N/A";

                // Check actual type
                auto vipT = std::dynamic_pointer_cast<VIPTicket>(ticket);
                auto stuT = std::dynamic_pointer_cast<StudentTicket>(ticket);

                if (vipT) {
                    typeStr = "VIP";
                    extraField = vipT->getVipGift();
                } else if (stuT) {
                    typeStr = "Student";
                    extraField = stuT->getStudentID();
                }

                file << ticket->getId() << ","
                     << ticket->getEventName() << ","
                     << ticket->getSeatNumber() << ","
                     << typeStr << ","
                     << ticket->getBuyerName() << ","
                     << ticket->calculatePrice() << ","
                     << extraField << "\n";
            }
        }
        file.close();
        return true;
    }
};

#endif // DATA_MANAGER_H
