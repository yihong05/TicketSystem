#ifndef VIP_TICKET_H
#define VIP_TICKET_H

#include "Ticket.h"

class VIPTicket : public Ticket {
private:
    std::string vipGift;
    double vipSurcharge;

public:
    VIPTicket(int id, const std::string& eventName, const std::string& seatNumber, double basePrice, 
              const std::string& gift = "官方紀念海報與限定手燈", double surcharge = 500.0)
        : Ticket(id, eventName, seatNumber, basePrice), vipGift(gift), vipSurcharge(surcharge) {}

    double calculatePrice() const override {
        // VIP price: basePrice * 1.5 + surcharge
        return basePrice * 1.5 + vipSurcharge;
    }

    std::string getTicketType() const override {
        return "VIP 尊榮票 (VIP)";
    }

    std::string getVipGift() const { return vipGift; }

    void displayInfo() const override {
        Ticket::displayInfo();
        std::cout << " [VIP 專屬權益] 贈送周邊: " << vipGift << "\n";
        std::cout << " [VIP 專屬權益] 額外福利: 專屬通道入場、VIP 貴賓休息室使用權\n";
        std::cout << "------------------------------------------\n";
    }
};

#endif // VIP_TICKET_H
