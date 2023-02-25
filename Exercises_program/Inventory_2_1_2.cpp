//
// Created by 郜事成 on 2023/2/8.
//
#include <mutex>
#include <thread>
#include <vector>
#include <set>
#include <iostream>
#include <unistd.h>

class Request;
class Inventory;


class Inventory {
public:
    void add(Request* req) {
        std::lock_guard<std::mutex> lg(mtx_);
        requests_.insert(req);
    }

    void remove(Request* req) {
        std::lock_guard<std::mutex> lg(mtx_);
        requests_.erase(req);
    }

    void printAll() const;


private:
    mutable std::mutex mtx_;
    std::set<Request*> requests_;
};

Inventory g_inventory;

class Request {
public:
    void process() {
        std::lock_guard<std::mutex> lg(mtx_);
        g_inventory.add(this);
    }

    ~Request() {
        std::cout<<"Request 析构"<<std::endl;
        std::lock_guard<std::mutex> lg(mtx_);
        std::cout<<"已获取Request锁"<<std::endl;
//        usleep(500 * 1000);
        g_inventory.remove(this);
    }

    void print() const {
        std::lock_guard<std::mutex> lg(mtx_);
        std::cout << "print() called." << std::endl;
    }

private:
    mutable std::mutex mtx_;
};

void Inventory::printAll() const {
    std::lock_guard<std::mutex> lg(mtx_);
    std::cout<<"已获取Inventory锁"<<std::endl;
    sleep(5);
    for (std::set<Request*>::const_iterator it = requests_.begin();
    it != requests_.end();
    ++it) {
        (*it)->print();
    }
    std::cout << "Inventory::printAll() unlocked" << std::endl;
}

void threadFunc() {
    Request* req = new Request;
    req->process();
    sleep(1);
    delete req;  //调用~Request，先获取Rquest的锁，再获取Inventory的锁
}

int main(int argc, char* argv[]) {
    std::thread t(threadFunc);
    t.detach();
    usleep(500 * 1000);
    g_inventory.printAll(); //先获取Inventory的锁，再获取Request的锁，所以造成死锁
}
