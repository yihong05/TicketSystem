#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include "Ticket.h"
#include "RegularTicket.h"
#include "VIPTicket.h"
#include "StudentTicket.h"
#include "DataManager.h"

// ANSI Color codes
const std::string RESET   = "\033[0m";
const std::string BOLD    = "\033[1m";
const std::string RED     = "\033[91m";
const std::string GREEN   = "\033[92m";
const std::string YELLOW  = "\033[93m";
const std::string CYAN    = "\033[96m";
const std::string MAGENTA = "\033[95m";
const std::string BG_RED  = "\033[97;41m";
const std::string BG_CYAN = "\033[30;106m";

// Helper to print banners
void printBanner(const std::string& title) {
    std::cout << MAGENTA << "================================================\n" << RESET;
    std::cout << BOLD << "     " << title << "\n" << RESET;
    std::cout << MAGENTA << "================================================\n" << RESET;
}

// Convert string to uppercase
std::string toUpper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::toupper(c); });
    return str;
}

// Validate that name doesn't contain comma and is not empty
bool validateName(const std::string& name) {
    if (name.empty()) {
        std::cout << RED << "錯誤：名稱不得為空！\n" << RESET;
        return false;
    }
    if (name.find(',') != std::string::npos) {
        std::cout << RED << "錯誤：名稱中不得包含逗號 ','！\n" << RESET;
        return false;
    }
    return true;
}

// Validate student ID
bool validateStudentID(const std::string& id) {
    if (id.empty()) {
        std::cout << RED << "錯誤：學號不得為空！\n" << RESET;
        return false;
    }
    if (id.find(',') != std::string::npos) {
        std::cout << RED << "錯誤：學號中不得包含逗號 ','！\n" << RESET;
        return false;
    }
    if (id.length() < 3) {
        std::cout << RED << "錯誤：學號長度過短，請輸入有效學號！\n" << RESET;
        return false;
    }
    return true;
}

// Render the visual seating map in the terminal with ANSI colors
void renderSeatingMap(const Event& event, const std::vector<std::shared_ptr<Ticket>>& tickets) {
    std::cout << "\n" << BOLD << "========== [" << event.name << "] 座位地圖 ==========" << RESET << "\n\n";
    
    // Stage representation
    std::cout << "                 " << BG_RED << "   【舞台 STAGE】   " << RESET << "\n\n";
    
    // Print column headers (numbers)
    std::cout << "     ";
    for (int c = 1; c <= event.cols; ++c) {
        std::cout << std::setw(6) << c;
    }
    std::cout << "\n";

    // Print rows
    for (int r = 0; r < event.rows; ++r) {
        char rowChar = 'A' + r;
        std::cout << "  " << rowChar << "  ";

        for (int c = 0; c < event.cols; ++c) {
            std::string seatNum = "";
            seatNum += rowChar;
            seatNum += std::to_string(c + 1);

            // Find ticket for this seat
            auto it = std::find_if(tickets.begin(), tickets.end(), [&](const std::shared_ptr<Ticket>& t) {
                return t->getEventName() == event.name && t->getSeatNumber() == seatNum;
            });

            if (it != tickets.end()) {
                if ((*it)->getIsBooked()) {
                    // Booked seats displayed in Red as [ XX ]
                    std::cout << RED << " [ XX ]" << RESET;
                } else {
                    // Available seats colored by ticket type
                    auto vip = std::dynamic_pointer_cast<VIPTicket>(*it);
                    auto stu = std::dynamic_pointer_cast<StudentTicket>(*it);

                    if (vip) {
                        std::cout << YELLOW << " [" << seatNum << ":V]" << RESET;
                    } else if (stu) {
                        std::cout << CYAN << " [" << seatNum << ":S]" << RESET;
                    } else {
                        std::cout << GREEN << " [" << seatNum << ":R]" << RESET;
                    }
                }
            } else {
                std::cout << " [ -- ]";
            }
        }
        std::cout << "\n";
    }

    std::cout << "\n" << BOLD << "【座位圖例說明】" << RESET << "\n";
    std::cout << YELLOW << " [座位:V] VIP尊榮票" << RESET << " (前排，享禮品與貴賓室)\n";
    std::cout << GREEN << " [座位:R] 一般票" << RESET << " (中排，標準價)\n";
    std::cout << CYAN << " [座位:S] 學生票" << RESET << " (後排，享8折優待)\n";
    std::cout << RED << " [ XX ]  該位置已售出\n" << RESET;
    std::cout << "================================================\n\n";
}

void customerMenu(DataManager& db) {
    while (true) {
        const auto& events = db.getEvents();
        printBanner("顧客服務系統 - 選擇活動");
        for (size_t i = 0; i < events.size(); ++i) {
            std::cout << " " << i + 1 << ". " << events[i].name 
                      << " (票價區間: $" << std::fixed << std::setprecision(0)
                      << events[i].basePrice * 0.8 << " ~ $" << events[i].basePrice * 1.5 + 500.0 << ")\n";
        }
        std::cout << " " << events.size() + 1 << ". 搜尋我的預購票券\n";
        std::cout << " " << events.size() + 2 << ". 返回主選單\n";
        std::cout << "------------------------------------------------\n";
        std::cout << "請輸入選項 (1-" << events.size() + 2 << "): ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cout << RED << "錯誤：無效輸入，請輸入數字！\n" << RESET;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == (int)events.size() + 2) {
            break; // Return to login role selection
        }

        if (choice == (int)events.size() + 1) {
            // Search bookings by customer name
            std::string buyerQuery;
            std::cout << "請輸入欲查詢的購買者姓名: ";
            std::cin >> buyerQuery;
            if (!validateName(buyerQuery)) {
                system("pause");
                continue;
            }

            std::cout << "\n========== 購買者 [" << buyerQuery << "] 的預購記錄 ==========\n";
            bool found = false;
            for (const auto& ticket : db.getTickets()) {
                if (ticket->getIsBooked() && ticket->getBuyerName() == buyerQuery) {
                    ticket->displayInfo();
                    found = true;
                }
            }
            if (!found) {
                std::cout << "查無任何預購記錄！\n";
            }
            std::cout << "======================================================\n";
            system("pause");
            continue;
        }

        if (choice < 1 || choice > (int)events.size()) {
            std::cout << RED << "錯誤：無效選項，超出活動範圍！\n" << RESET;
            continue;
        }

        const Event& selectedEvent = events[choice - 1];

        while (true) {
            std::cout << "\n--- 活動選單: [" << selectedEvent.name << "] ---\n";
            std::cout << " 1. 檢視視覺化座位圖與空位狀態\n";
            std::cout << " 2. 預購門票\n";
            std::cout << " 3. 取消預購 (退票)\n";
            std::cout << " 4. 返回活動列表\n";
            std::cout << "------------------------------------------------\n";
            std::cout << "請選擇操作 (1-4): ";

            int actChoice = 0;
            if (!(std::cin >> actChoice)) {
                std::cout << RED << "錯誤：無效輸入，請輸入數字！\n" << RESET;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            if (actChoice == 4) break;

            auto& tickets = db.getTickets();

            if (actChoice == 1) {
                renderSeatingMap(selectedEvent, tickets);
            } 
            else if (actChoice == 2) {
                renderSeatingMap(selectedEvent, tickets);
                std::cout << "請選擇欲預購的座位 (例如 A1, B3): ";
                std::string seatNum;
                std::cin >> seatNum;
                seatNum = toUpper(seatNum); // Case-insensitive input validation

                auto it = std::find_if(tickets.begin(), tickets.end(), [&](const std::shared_ptr<Ticket>& t) {
                    return t->getEventName() == selectedEvent.name && t->getSeatNumber() == seatNum;
                });

                if (it == tickets.end()) {
                    std::cout << RED << "錯誤：在此活動中找不到座位號碼 \"" << seatNum << "\"，請檢查輸入！\n" << RESET;
                } else if ((*it)->getIsBooked()) {
                    std::cout << RED << "錯誤：此座位已被預訂 (購買者: " << (*it)->getBuyerName() << ")！\n" << RESET;
                } else {
                    // Display details of the seat before booking
                    std::cout << "\n[ 您選擇的座位資訊 ]\n";
                    std::cout << " 票種類型: " << (*it)->getTicketType() << "\n";
                    std::cout << " 計算票價: $" << (*it)->calculatePrice() << " TWD\n";
                    
                    auto vip = std::dynamic_pointer_cast<VIPTicket>(*it);
                    if (vip) {
                        std::cout << " VIP 贈品: " << vip->getVipGift() << "\n";
                    }

                    std::cout << "\n確定要預購嗎？(Y/N): ";
                    std::string confirmStr;
                    std::cin >> confirmStr;
                    confirmStr = toUpper(confirmStr);
                    if (confirmStr != "Y") {
                        std::cout << "已取消預購操作。\n";
                        continue;
                    }

                    std::string buyerName;
                    std::cout << "請輸入購買者姓名: ";
                    std::cin >> buyerName;
                    if (!validateName(buyerName)) {
                        system("pause");
                        continue;
                    }

                    auto studentTicket = std::dynamic_pointer_cast<StudentTicket>(*it);
                    if (studentTicket) {
                        std::string studentId;
                        std::cout << "此票為學生票，請輸入學號進行綁定: ";
                        std::cin >> studentId;
                        if (!validateStudentID(studentId)) {
                            system("pause");
                            continue;
                        }
                        studentTicket->setStudentID(studentId);
                    }

                    (*it)->book(buyerName);
                    if (db.saveBookings()) {
                        std::cout << GREEN << "\n【成功】預訂成功！已即時同步至 records。\n" << RESET;
                        (*it)->displayInfo();
                    } else {
                        std::cout << RED << "警告：預訂成功，但寫入 bookings.txt 檔案失敗！\n" << RESET;
                    }
                }
                system("pause");
            } 
            else if (actChoice == 3) {
                std::cout << "請輸入欲取消的座位 (例如 A1, B3): ";
                std::string seatNum;
                std::cin >> seatNum;
                seatNum = toUpper(seatNum);

                auto it = std::find_if(tickets.begin(), tickets.end(), [&](const std::shared_ptr<Ticket>& t) {
                    return t->getEventName() == selectedEvent.name && t->getSeatNumber() == seatNum;
                });

                if (it == tickets.end()) {
                    std::cout << RED << "錯誤：找不到該座位！\n" << RESET;
                } else if (!(*it)->getIsBooked()) {
                    std::cout << RED << "錯誤：該座位目前為空閒狀態，無須退票！\n" << RESET;
                } else {
                    std::cout << "\n[ 欲取消的票券資訊 ]\n";
                    (*it)->displayInfo();
                    std::cout << "\n確定要取消此預定並釋出座位嗎？(Y/N): ";
                    std::string confirmStr;
                    std::cin >> confirmStr;
                    confirmStr = toUpper(confirmStr);
                    if (confirmStr != "Y") {
                        std::cout << "退票操作已中止。\n";
                        continue;
                    }

                    // Reset to clean ticket of correct type
                    int ticketId = (*it)->getId();
                    double basePrice = (*it)->getBasePrice();
                    std::shared_ptr<Ticket> resetTicket = nullptr;

                    auto vip = std::dynamic_pointer_cast<VIPTicket>(*it);
                    auto stu = std::dynamic_pointer_cast<StudentTicket>(*it);

                    if (vip) {
                        resetTicket = std::make_shared<VIPTicket>(ticketId, selectedEvent.name, seatNum, basePrice);
                    } else if (stu) {
                        resetTicket = std::make_shared<StudentTicket>(ticketId, selectedEvent.name, seatNum, basePrice);
                    } else {
                        resetTicket = std::make_shared<RegularTicket>(ticketId, selectedEvent.name, seatNum, basePrice);
                    }

                    *it = resetTicket;
                    if (db.saveBookings()) {
                        std::cout << GREEN << "\n【成功】退票成功！座位已重新釋出，記錄已同步更新。\n" << RESET;
                    } else {
                        std::cout << RED << "警告：退票成功，但存檔失敗！\n" << RESET;
                    }
                }
                system("pause");
            } else {
                std::cout << RED << "錯誤：無效操作！請輸入 1 至 4 的數字。\n" << RESET;
            }
        }
    }
}

void adminMenu(DataManager& db) {
    // Basic passcode verification
    std::cout << "請輸入管理員密碼: ";
    std::string pass;
    std::cin >> pass;
    if (pass != "admin123") {
        std::cout << RED << "密碼錯誤！拒絕存取。\n" << RESET;
        system("pause");
        return;
    }

    while (true) {
        printBanner("管理員後台控制系統");
        std::cout << " 1. 檢視所有活動之座位銷售明細\n";
        std::cout << " 2. 查看系統營收與票種銷售統計 (視覺統計)\n";
        std::cout << " 3. 重設所有預約記錄 (清空資料)\n";
        std::cout << " 4. 返回主選單\n";
        std::cout << "------------------------------------------------\n";
        std::cout << "請選擇功能 (1-4): ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cout << RED << "無效輸入！請重新選擇。\n" << RESET;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 4) break;

        auto& tickets = db.getTickets();
        const auto& events = db.getEvents();

        if (choice == 1) {
            std::cout << "\n========== 全系統座位訂單明細 ==========\n";
            int soldCount = 0;
            for (const auto& ticket : tickets) {
                if (ticket->getIsBooked()) {
                    ticket->displayInfo();
                    soldCount++;
                }
            }
            std::cout << "\n目前全系統總共售出: " << soldCount << " 張門票。\n";
            std::cout << "========================================\n";
            system("pause");
        } 
        else if (choice == 2) {
            // Advanced Sales breakdown and visual capacity stats
            double totalRevenue = 0.0;
            int regularSold = 0, vipSold = 0, studentSold = 0;

            std::cout << "\n========== 1. 活動座位銷售率統計 ==========\n";
            for (const auto& ev : events) {
                int evTotal = 0;
                int evSold = 0;
                for (const auto& t : tickets) {
                    if (t->getEventName() == ev.name) {
                        evTotal++;
                        if (t->getIsBooked()) {
                            evSold++;
                            totalRevenue += t->calculatePrice();

                            auto vip = std::dynamic_pointer_cast<VIPTicket>(t);
                            auto stu = std::dynamic_pointer_cast<StudentTicket>(t);

                            if (vip) vipSold++;
                            else if (stu) studentSold++;
                            else regularSold++;
                        }
                    }
                }
                double rate = (evTotal > 0) ? (double)evSold / evTotal * 100.0 : 0.0;
                std::cout << " " << std::left << std::setw(22) << ev.name 
                          << " | 銷售量: " << std::right << std::setw(2) << evSold << "/" << std::setw(2) << evTotal 
                          << " | 銷售率: " << std::fixed << std::setprecision(1) << rate << " %\n";
            }

            int totalSold = regularSold + vipSold + studentSold;
            int systemTotal = (int)tickets.size();
            double systemRate = (systemTotal > 0) ? (double)totalSold / systemTotal * 100.0 : 0.0;

            std::cout << "\n========== 2. 票種銷售明細統計 ==========\n";
            std::cout << "  - 一般票 (Regular) : " << regularSold << " 張\n";
            std::cout << "  - VIP尊榮票 (VIP)  : " << vipSold << " 張\n";
            std::cout << "  - 學生票 (Student) : " << studentSold << " 張\n";
            std::cout << "  ------------------------------------------\n";
            std::cout << "  - 全系統總銷售量   : " << totalSold << " / " << systemTotal << " 張 (" << systemRate << " %)\n\n";
            std::cout << BOLD << " [系統總營業收入]    : $" << std::fixed << std::setprecision(0) << totalRevenue << " TWD\n" << RESET;
            std::cout << "============================================\n";
            system("pause");
        } 
        else if (choice == 3) {
            std::cout << RED << "\n【高度警告】確定要重設所有的預定記錄嗎？此動作將清除所有訂單！(Y/N): " << RESET;
            std::string confirmStr;
            std::cin >> confirmStr;
            confirmStr = toUpper(confirmStr);
            if (confirmStr == "Y") {
                db.loadEvents(); // Load blank layout templates
                if (db.saveBookings()) {
                    std::cout << GREEN << "已成功清空所有預定資料，系統恢復初始狀態！\n" << RESET;
                } else {
                    std::cout << RED << "錯誤：重設存檔失敗！\n" << RESET;
                }
            } else {
                std::cout << "已取消重設操作。\n";
            }
            system("pause");
        } else {
            std::cout << RED << "錯誤：請選擇 1 至 4 之間的功能選項！\n" << RESET;
        }
    }
}

int main() {
    // Configure terminal encoding to UTF-8
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif

    DataManager db("data/events.txt", "data/bookings.txt");
    if (!db.loadEvents()) {
        std::cerr << "系統初始化失敗：找不到或無法讀取 events.txt 活動資料檔！\n";
        system("pause");
        return 1;
    }
    if (!db.loadBookings()) {
        std::cerr << "警告：無法載入 bookings.txt 訂單資料！\n";
    }

    while (true) {
        printBanner("門票預購系統主介面");
        std::cout << "請選擇登入身分:\n";
        std::cout << " 1. 顧客模式 (Customer Mode)\n";
        std::cout << " 2. 管理員後台 (Admin Mode)\n";
        std::cout << " 3. 離開預購系統 (Exit)\n";
        std::cout << "------------------------------------------------\n";
        std::cout << "請輸入選擇 (1-3): ";

        int role = 0;
        if (!(std::cin >> role)) {
            std::cout << RED << "錯誤：無效的輸入，請輸入數字！\n" << RESET;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (role == 3) {
            std::cout << GREEN << "\n感謝您使用本系統，祝您購票愉快！再見！\n" << RESET;
            break;
        }

        if (role == 1) {
            customerMenu(db);
        } else if (role == 2) {
            adminMenu(db);
        } else {
            std::cout << RED << "錯誤：無效選項，請輸入 1, 2 或 3。\n" << RESET;
            system("pause");
        }
    }

    return 0;
}
