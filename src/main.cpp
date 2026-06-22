#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "Ticket.h"
#include "RegularTicket.h"
#include "VIPTicket.h"
#include "StudentTicket.h"

void printMenu() {
    std::cout << "\n==========================================\n";
    std::cout << "     門票預購系統 (v1.0 記憶體測試版)     \n";
    std::cout << "==========================================\n";
    std::cout << " 1. 顯示所有座位/票券資訊 (多型展示)\n";
    std::cout << " 2. 進行預購門票\n";
    std::cout << " 3. 取消預購門票 (退票)\n";
    std::cout << " 4. 退出系統\n";
    std::cout << "==========================================\n";
    std::cout << "請輸入您的選擇 (1-4): ";
}

int main() {
    // 設定終端機支援 UTF-8 編碼 (Windows 環境下)
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif

    // 使用 STL std::vector 與 std::shared_ptr 來儲存多型物件
    std::vector<std::shared_ptr<Ticket>> tickets;

    // 建立一些測試用的活動門票 (周杰倫演唱會，基礎票價 $3000)
    std::string eventName = "周杰倫台北演唱會";
    double basePrice = 3000.0;

    // 填入初始座位資料
    tickets.push_back(std::make_shared<RegularTicket>(101, eventName, "A1", basePrice));
    tickets.push_back(std::make_shared<RegularTicket>(102, eventName, "A2", basePrice));
    tickets.push_back(std::make_shared<VIPTicket>(201, eventName, "VIP-1", basePrice, "周杰倫限定紀念海報 + 螢光棒", 1000.0));
    tickets.push_back(std::make_shared<VIPTicket>(202, eventName, "VIP-2", basePrice, "周杰倫限定紀念海報 + 螢光棒", 1000.0));
    tickets.push_back(std::make_shared<StudentTicket>(301, eventName, "S1", basePrice));
    tickets.push_back(std::make_shared<StudentTicket>(302, eventName, "S2", basePrice));

    int choice = 0;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cout << "無效的輸入！請輸入數字。\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 4) {
            std::cout << "\n感謝使用門票預購系統，再見！\n";
            break;
        }

        switch (choice) {
            case 1: {
                std::cout << "\n========== 全體票券資訊 (多型展示) ==========\n";
                for (const auto& ticket : tickets) {
                    ticket->displayInfo();
                }
                break;
            }
            case 2: {
                int targetId;
                std::cout << "請輸入欲預購的票券 ID: ";
                if (!(std::cin >> targetId)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "無效的 ID 格式！\n";
                    break;
                }

                // 使用 STL std::find_if 來查詢票券
                auto it = std::find_if(tickets.begin(), tickets.end(), [targetId](const std::shared_ptr<Ticket>& t) {
                    return t->getId() == targetId;
                });

                if (it == tickets.end()) {
                    std::cout << "錯誤：找不到 ID 為 " << targetId << " 的票券。\n";
                } else if ((*it)->getIsBooked()) {
                    std::cout << "錯誤：該門票已被預購 (購買者: " << (*it)->getBuyerName() << ")。\n";
                } else {
                    std::string buyerName;
                    std::cout << "請輸入購買者姓名: ";
                    std::cin >> buyerName;

                    // 使用 std::dynamic_pointer_cast 進行安全的向下轉型 (Downcasting)
                    // 以處理學生票專有的學號輸入
                    auto studentTicket = std::dynamic_pointer_cast<StudentTicket>(*it);
                    if (studentTicket) {
                        std::string studentId;
                        std::cout << "此票券為學生票，請輸入學號進行驗證: ";
                        std::cin >> studentId;
                        studentTicket->setStudentID(studentId);
                    }

                    (*it)->book(buyerName);
                    std::cout << "預約成功！\n";
                    std::cout << "您已預約：" << (*it)->getSeatNumber() << " 號座位\n";
                    std::cout << "應付金額為：$" << std::fixed << std::setprecision(0) << (*it)->calculatePrice() << " TWD\n";
                }
                break;
            }
            case 3: {
                int targetId;
                std::cout << "請輸入要取消預購的票券 ID: ";
                if (!(std::cin >> targetId)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "無效的 ID 格式！\n";
                    break;
                }

                auto it = std::find_if(tickets.begin(), tickets.end(), [targetId](const std::shared_ptr<Ticket>& t) {
                    return t->getId() == targetId;
                });

                if (it == tickets.end()) {
                    std::cout << "錯誤：找不到 ID 為 " << targetId << " 的票券。\n";
                } else if (!(*it)->getIsBooked()) {
                    std::cout << "錯誤：該票券尚未售出，無法取消預購。\n";
                } else {
                    // 若為學生票，取消時一併清空學號
                    auto studentTicket = std::dynamic_pointer_cast<StudentTicket>(*it);
                    if (studentTicket) {
                        studentTicket->setStudentID("");
                    }
                    (*it)->cancel();
                    std::cout << "成功！已取消該門票預購，座位已釋出。\n";
                }
                break;
            }
            default:
                std::cout << "請輸入 1 到 4 之間的選項。\n";
        }
    }

    return 0;
}
