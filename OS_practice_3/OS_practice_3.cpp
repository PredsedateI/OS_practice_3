#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <Windows.h>

class goods;
void produce();
void consume();
void clearConsole();

goods* first = nullptr;
goods* last = nullptr;
int numberOfGoods = 0;
std::mutex queueGuard;
bool breaked = false;

class goods {
public:
    goods* prev;
    int number;
    goods* next;
    goods() {
        prev = nullptr;
        number = rand() % 100 + 1;
        next = nullptr;
    }
};

int main() {
    srand(time(0));
    
    for (;;) {
        breaked = false;

        int numberOfProducers = 0, numberOfConsumers = 0;

        std::cout << "Type \"1\" for default settings, \"2\" for advanced or \"q\" for quit\n";
    choise1:
        char typed = _getch();
        if (typed == '1') {
            numberOfProducers = 3;
            numberOfConsumers = 2;
            std::cout << "Number of producers set to " << numberOfProducers << ", number of consumers set to " << numberOfConsumers << '\n';
        }
        else if (typed == '2') {
            std::cout << "Enter producers amount: ";
            std::cin >> numberOfProducers;
            std::cout << "Enter consumers amount: ";
            std::cin >> numberOfConsumers;
        }
        else if (typed == 'q') {
            break;
        }
        else {
            goto choise1;
        }

        std::thread* producer = new std::thread[numberOfProducers];
        for (int i = 0; i < numberOfProducers; i++) producer[i] = std::thread(produce);
        std::thread* consumer = new std::thread[numberOfConsumers];
        for (int i = 0; i < numberOfConsumers; i++) consumer[i] = std::thread(consume);

        std::cout << "Type \"q\" for quit or \"r\" for restart\n";
    choise2:
        typed = _getch();
        if (typed == 'q') {
            breaked = true;
            std::cout << "Waiting for consumers consumption what they should consume...\n";
            for (int i = 0; i < numberOfConsumers; i++) {
                consumer[i].join();
            }
            break;
        }
        else if (typed == 'r') {
            breaked = true;
            std::cout << "Waiting for consumers consumption what they should consume...\n";
            for (int i = 0; i < numberOfConsumers; i++) {
                consumer[i].join();
            }
            clearConsole();
            continue;
        }
        else {
            goto choise2;
        }
    }
    return 0;
}

void produce() {
    for (; !breaked;) {
        std::lock_guard<std::mutex> mutexGuard(queueGuard);
        goods* a = new goods;
        if (last) {
            last->next = a;
            a->prev = last;
        } else {
            first = a;
        }
        last = a;
        numberOfGoods++;
    }
    return;
}

void consume() {
    for (; !breaked or numberOfGoods > 0;) {
        std::lock_guard<std::mutex> mutexGuard(queueGuard);
        if (first) {
            goods* a = first;
            if (first->next) {
                first->next->prev = nullptr;
                first = first->next;
            } else {
                first = nullptr;
                last = nullptr;
            }
            delete a;
            numberOfGoods--;
        }
    }
    return;
}

void clearConsole() {
    COORD cur;
    cur.X = 0;
    cur.Y = 0;
    HANDLE handleOfConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handleOfConsole, cur);
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
    GetConsoleScreenBufferInfo(handleOfConsole, &consoleScreenBufferInfo);
    cur.X = consoleScreenBufferInfo.dwSize.X;
    cur.Y = 180;
    char* spaces = new char[cur.X * cur.Y + 1];
    for (int i = 0; i < cur.X * cur.Y; i++) spaces[i] = ' ';
    spaces[cur.X * cur.Y] = '\0';
    std::cout << spaces;
    cur.X = 0;
    cur.Y = 0;
    SetConsoleCursorPosition(handleOfConsole, cur);
    return;
}