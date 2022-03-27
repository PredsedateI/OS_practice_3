#include <conio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <Windows.h>
#include <WinUser.h>

class goods;
void produce();
void consume();
void clearConsole();
COORD getConsoleCursorPos();
void setConsoleCursorVisible(bool a);
void draw(int value);
void undraw();
void drawCounter();
void endOfCycle();
char QorR();

goods* first = nullptr;
goods* last = nullptr;
int numberOfGoods = 0;
std::mutex queueGuard;
int sleepDelay = 0;
bool showCounter = false;
bool breaked = false;
bool sleep = false;
int sleepTrigger = 0;
int awakeTrigger = 0;
bool output = false;
std::mutex coutGuard;
COORD beginPos;
COORD firstPos;
COORD lastPos;

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
    setConsoleCursorVisible(false);
    for (;;) {
        int numberOfProducers = 0, numberOfConsumers = 0;
    choise1:
        std::cout << "Type \"1\" for default settings, \"2\" for advanced or \"q\" for quit\n";
        char typed = _getch();
        clearConsole();
        if (typed == '1') {
            numberOfProducers = 3;
            numberOfConsumers = 2;
            sleepTrigger = 100;
            awakeTrigger = 80;
            sleepDelay = 80;
            showCounter = true;
            std::cout << "Number of producers set to " << numberOfProducers << ", number of consumers set to " << numberOfConsumers << '\n';
        }
        else if (typed == '2') {
            setConsoleCursorVisible(true);
            std::cout << "Enter producers amount: ";
            std::cin >> numberOfProducers;
            std::cout << "Enter consumers amount: ";
            std::cin >> numberOfConsumers;
            std::cout << "Enter producers sleep trigger: ";
            std::cin >> sleepTrigger;
            std::cout << "Enter producers awake trigger: ";
            std::cin >> awakeTrigger;
            std::cout << "Enter min time (in milliseconds) of produce/consume: ";
            std::cin >> sleepDelay;
            setConsoleCursorVisible(false);
            std::cout << "Show counter? (y/n)\n";
        choise2:
            typed = _getch();
            if (typed == 'y') showCounter = true;
            else if (typed == 'n') showCounter = false;
            else goto choise2;
        }
        else if (typed == 'q') {
            break;
        }
        else goto choise1;

        breaked = false;
        output = false;

        if (180 < sleepTrigger) {
            std::cout << "The queue may be too large to display\n";
        }
        std::cout << '\n';
        beginPos = getConsoleCursorPos();
        firstPos = beginPos;
        lastPos = beginPos;
        if (180 >= sleepTrigger) {
            output = true;
            for (int i = 0; i < 20; i++) std::cout << '\n';
        }
        std::cout << "Current number of goods: ";
        if (!showCounter) std::cout << "N/A";
        std::cout << "\n\nType \"q\" for quit or \"r\" for restart\n";

        std::thread* producer = new std::thread[numberOfProducers];
        for (int i = 0; i < numberOfProducers; i++) producer[i] = std::thread(produce);
        std::thread* consumer = new std::thread[numberOfConsumers];
        for (int i = 0; i < numberOfConsumers; i++) consumer[i] = std::thread(consume);

        typed = QorR();
        if (typed == 'q') {
            endOfCycle();
            for (int i = 0; i < numberOfConsumers; i++) {
                consumer[i].join();
            }
            break;
        }
        else if (typed == 'r') {
            endOfCycle();
            for (int i = 0; i < numberOfConsumers; i++) {
                consumer[i].join();
            }
            clearConsole();
            continue;
        }
    }
    return 0;
}

void produce() {
    for (;;) {
        std::lock_guard<std::mutex> mutexGuard(queueGuard);
        if (sleep) continue;
        if (breaked) break;
        goods* a = new goods;
        if (last) {
            last->next = a;
            a->prev = last;
        } else {
            first = a;
        }
        last = a;
        numberOfGoods++;
        if (output) draw(last->number);
        if (showCounter) drawCounter();
        if (numberOfGoods >= sleepTrigger) sleep = true;
        Sleep(sleepDelay);
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
            if (output) undraw();
            if (showCounter) drawCounter();
            if (numberOfGoods <= awakeTrigger) sleep = false;
            Sleep(sleepDelay);
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
    PCONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = new CONSOLE_SCREEN_BUFFER_INFO;
    GetConsoleScreenBufferInfo(handleOfConsole, consoleScreenBufferInfo);
    cur.X = consoleScreenBufferInfo->dwSize.X;
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

COORD getConsoleCursorPos() {
    PCONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = new CONSOLE_SCREEN_BUFFER_INFO;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), consoleScreenBufferInfo);
    return consoleScreenBufferInfo->dwCursorPosition;
}

void setConsoleCursorVisible(bool a) {
    HANDLE handleOfConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO b;
    GetConsoleCursorInfo(handleOfConsole, &b);
    b.bVisible = a;
    SetConsoleCursorInfo(handleOfConsole, &b);
}

void draw(int value) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), lastPos);
    std::cout << "  " << std::setw(3) << value << ' ';
    if (114 > getConsoleCursorPos().X) {
        lastPos.X += 6;
    }
    else {
        lastPos.X = 0;
        lastPos.Y += 2;
        if (lastPos.Y > beginPos.Y + 18) lastPos.Y = beginPos.Y;
    }
    return;
}

void undraw() {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), firstPos);
    std::cout << "      ";
    if (114 > getConsoleCursorPos().X) {
        firstPos.X += 6;
    }
    else {
        firstPos.X = 0;
        firstPos.Y += 2;
        if (firstPos.Y > beginPos.Y + 18) firstPos.Y = beginPos.Y;
    }
    return;
}

void drawCounter() {
    HANDLE handleOfConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD goodsCounterPos;
    goodsCounterPos.X = 25;
    goodsCounterPos.Y = beginPos.Y;
    if (output) goodsCounterPos.Y += 20;
    std::lock_guard<std::mutex> mutexGuard(coutGuard);
    SetConsoleCursorPosition(handleOfConsole, goodsCounterPos);
    std::cout << numberOfGoods << ' ';
    goodsCounterPos.X = 0;
    goodsCounterPos.Y += 4;
    SetConsoleCursorPosition(handleOfConsole, goodsCounterPos);
    return;
}

void endOfCycle() {
    breaked = true;
    COORD cur = beginPos;
    if (output) cur.Y += 23;
    else cur.Y += 3;
    std::lock_guard<std::mutex> mutexGuard(coutGuard);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cur);
    std::cout << "Waiting for consumers consumption what they should consume...\n";
    return;
}

char QorR() {
    while (true) {
        if (GetKeyState(81) & 32768) return 'q';
        if (GetKeyState(82) & 32768) return 'r';
    }
}