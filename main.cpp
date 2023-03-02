//
// Created by scgao on 2023/2/18.
//

#include <iostream>
#include <memory>
#include "util/BlockingQueue.h"
#include <unistd.h>
#include <stdlib.h>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <queue>

using namespace std;

struct Entry{
public:
    Entry(int id = 0, double width = 18.0, bool isEmergency = false)
            : id(id), width(width), isEmgercy(isEmergency) {  };

    int id;
    double width;
    bool isEmgercy;
};



int main(int argc,char* argv[]) {
    queue<double> q;
    cout<< sizeof(struct timeval)<<endl;
}
