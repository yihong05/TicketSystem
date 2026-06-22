#ifndef STUDENT_TICKET_H
#define STUDENT_TICKET_H

#include "Ticket.h"

class StudentTicket : public Ticket {
private:
    std::string studentID;
    double discountRate;

public:
    StudentTicket(int id, const std::string& eventName, const std::string& seatNumber, double basePrice,
                  const std::string& stuID = "")
        : Ticket(id, eventName, seatNumber, basePrice), studentID(stuID), discountRate(0.8) {}

    double calculatePrice() const override {
        // Student ticket gets 20% discount (0.8x basePrice)
        return basePrice * discountRate;
    }

    std::string getTicketType() const override {
        return "學生優待票 (Student)";
    }

    std::string getStudentID() const { return studentID; }
    void setStudentID(const std::string& id) { studentID = id; }

    void displayInfo() const override {
        Ticket::displayInfo();
        std::cout << " [學生專屬憑證] 認證學號: " << (studentID.empty() ? "尚未驗證" : studentID) << " (入場請攜帶學生證)\n";
        std::cout << "------------------------------------------\n";
    }
};

#endif // STUDENT_TICKET_H
