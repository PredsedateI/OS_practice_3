#include <thread>
#include <mutex>
#include <iostream>
#include <random>
#include <ctime>
#include <Windows.h>

class tovar;
void produce();
void consume();

tovar* first;
tovar* last;
int amount;
std::mutex queueGuard;

class tovar {
public:
    tovar* prev;
    int number;
    tovar* next;
    tovar() {
        prev = NULL;
        number = rand() % 100 + 1;
        next = NULL;
    }
    ~tovar() {
        delete prev;
    }
};

int main() {
    srand(time(0));

    //tovar* firstlast = new tovar;
    //first = firstlast;
    //last = firstlast;
    //amount = 1;

    first = NULL;
    last = NULL;
    amount = 0;
    
    std::thread fabricator0(produce);
    std::thread fabricator1(produce);
    std::thread fabricator2(produce);
    std::thread consumer0(consume);
    std::thread consumer1(consume);
    std::thread consumer2(consume);
    std::thread consumer3(consume);
    std::thread consumer4(consume);
    std::thread consumer5(consume);
    std::thread consumer6(consume);

    fabricator0.join();
    return 0;
}

void produce() {
    for (;;) {
        queueGuard.lock();
        tovar* a = new tovar;
        if (last) {
            last->next = a;
            std::cout << a << '\n';
            a->prev = last;
        } else {
            first = a;
        }
        last = a;
        amount++;
        std::cout << last->prev << ' ' << last << ' ';
        //std::cout << last->number << '\n';
        queueGuard.unlock();
    }
}

void consume() {
    for (;;) {
        queueGuard.lock();
        if (first) {
            tovar* a = first;
            if (first->next) {
                first->next->prev = NULL;
                first = first->next;
            } else {
                last = 0;
            }
            delete a;
            amount--;
        }
        queueGuard.unlock();
    }
}