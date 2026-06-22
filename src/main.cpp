#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "Ticket.h"
#include "RegularTicket.h"
#include "VIPTicket.h"
#include "StudentTicket.h"
#include "DataManager.h"

void printEventMenu(const std::vector<Event>& events) {
    std::cout << "\n==========================================\n";
    std::cout << "        門票預購系統 (v2.0 檔案存取版)     \n";
    std::cout << "==========================================\n";
    std::cout << " 可預購活動列表:\n";
    for (size_t i = 0; i < events.size(); ++i) {
        std::cout << " " << i + 1 << ". " << events[i].name 
                  << " (起價: $" << events[i].basePrice << " TWD)\n";
    }
    std::cout << " " << events.size() + 1 << ". 離開系統\n";
    std::cout << "==========================================\n";
    std::cout << "請選擇活動項目 (1-" << events.size() + 1 << "): ";
}

void printActionMenu(const std::string& eventName) {
    std::cout << "\n--- 活動: [" << eventName << "] ---\n";
    std::cout << " 1. 檢視所有座位與售票資訊\n";
    std::cout << " 2. 預購此活動門票 (一般票/VIP票/學生票)\n";
    std::cout << " 3. 取消預購門票 (退票)\n";
    std::cout << " 4. 返回主選單\n";
    std::cout << "------------------------------------------\n";
    std::cout << "請選擇操作 (1-4): ";
}

int main() {
    // 設定終端機支援 UTF-8 編碼 (Windows 環境下)
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif

    DataManager db("data/events.txt", "data/bookings.txt");
    if (!db.loadEvents()) {
        std::cerr << "系統初始化失敗，無法讀取活動檔！\n";
        return 1;
    }
    if (!db.loadBookings()) {
        std::cerr << "警告：無法載入預訂紀錄！\n";
    }

    while (true) {
        const auto& events = db.getEvents();
        printEventMenu(events);
        int eventChoice = 0;
        if (!(std::cin >> eventChoice)) {
            std::cout << "無效輸入，請輸入數字！\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (eventChoice == (int)events.size() + 1) {
            std::cout << "\n感謝使用門票預購系統，再見！\n";
            break;
        }

        if (eventChoice < 1 || eventChoice > (int)events.size()) {
            std::cout << "選擇超出範圍！\n";
            continue;
        }

        const Event& selectedEvent = events[eventChoice - 1];

        while (true) {
            printActionMenu(selectedEvent.name);
            int actionChoice = 0;
            if (!(std::cin >> actionChoice)) {
                std::cout << "無效輸入，請輸入數字！\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            if (actionChoice == 4) {
                break;
            }

            auto& tickets = db.getTickets();

            switch (actionChoice) {
                case 1: {
                    std::cout << "\n========== [" << selectedEvent.name << "] 座位狀態 ==========\n";
                    for (const auto& ticket : tickets) {
                        if (ticket->getEventName() == selectedEvent.name) {
                            ticket->displayInfo();
                        }
                    }
                    break;
                }
                case 2: {
                    std::cout << "\n請輸入要預購的座位號碼 (例如 A1, B2): ";
                    std::string seatNum;
                    std::cin >> seatNum;

                    // 尋找對應的座位
                    auto it = std::find_if(tickets.begin(), tickets.end(), [&](const std::shared_ptr<Ticket>& t) {
                        return t->getEventName() == selectedEvent.name && t->getSeatNumber() == seatNum;
                    });

                    if (it == tickets.end()) {
                        std::cout << "錯誤：在此活動中找不到座位號碼 \"" << seatNum << "\"\n";
                    } else if ((*it)->getIsBooked()) {
                        std::cout << "錯誤：座位 \"" << seatNum << "\" 已經被預購！\n";
                    } else {
                        std::cout << "請選擇票券種類:\n";
                        std::cout << " 1. 一般票 (原價 $" << selectedEvent.basePrice << ")\n";
                        std::cout << " 2. VIP 尊榮票 (1.5倍價格 + $500 附加費，贈送專屬周邊)\n";
                        std::cout << " 3. 學生優待票 (享 8 折優惠，需驗證學生證學號)\n";
                        std::cout << "請輸入票種選項 (1-3): ";
                        int typeChoice = 1;
                        std::cin >> typeChoice;

                        std::string buyerName;
                        std::cout << "請輸入購買者姓名: ";
                        std::cin >> buyerName;

                        std::shared_ptr<Ticket> newTicket = nullptr;
                        int ticketId = (*it)->getId();

                        if (typeChoice == 2) {
                            newTicket = std::make_shared<VIPTicket>(ticketId, selectedEvent.name, seatNum, selectedEvent.basePrice);
                        } else if (typeChoice == 3) {
                            std::string stuId;
                            std::cout << "請輸入學生證學號: ";
                            std::cin >> stuId;
                            newTicket = std::make_shared<StudentTicket>(ticketId, selectedEvent.name, seatNum, selectedEvent.basePrice, stuId);
                        } else {
                            newTicket = std::make_shared<RegularTicket>(ticketId, selectedEvent.name, seatNum, selectedEvent.basePrice);
                        }

                        if (newTicket) {
                            newTicket->book(buyerName);
                            *it = newTicket; // 用新建立的衍生票種物件替換原本的預設一般票

                            // 寫入檔案儲存
                            if (db.saveBookings()) {
                                std::cout << "預約成功！訂單已即時同步至 bookings.txt。\n";
                                std::cout << "應付票價: $" << newTicket->calculatePrice() << " TWD\n";
                            } else {
                                std::cout << "警告：預約成功，但寫入檔案時失敗！\n";
                            }
                        }
                    }
                    break;
                }
                case 3: {
                    std::cout << "\n請輸入欲取消的座位號碼 (例如 A1, B2): ";
                    std::string seatNum;
                    std::cin >> seatNum;

                    auto it = std::find_if(tickets.begin(), tickets.end(), [&](const std::shared_ptr<Ticket>& t) {
                        return t->getEventName() == selectedEvent.name && t->getSeatNumber() == seatNum;
                    });

                    if (it == tickets.end()) {
                        std::cout << "錯誤：在此活動中找不到該座位！\n";
                    } else if (!(*it)->getIsBooked()) {
                        std::cout << "錯誤：該座位目前為空閒狀態，無法取消！\n";
                    } else {
                        // 重置為預設的空閒 RegularTicket
                        int ticketId = (*it)->getId();
                        auto resetTicket = std::make_shared<RegularTicket>(ticketId, selectedEvent.name, seatNum, selectedEvent.basePrice);
                        *it = resetTicket;

                        // 寫入檔案儲存
                        if (db.saveBookings()) {
                            std::cout << "退票成功！座位已釋出，bookings.txt 已同步更新。\n";
                        } else {
                            std::cout << "警告：退票成功，但寫入檔案時失敗！\n";
                        }
                    }
                    break;
                }
                default:
                    std::cout << "無效操作選項！\n";
            }
        }
    }

    return 0;
}
