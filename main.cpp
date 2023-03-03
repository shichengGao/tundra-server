//
// Created by scgao on 2023/2/18.
//

#include <iostream>
#include <memory>
#include "util/BlockingQueue.h"
#include <unistd.h>
#include <cstring>
#include <stdlib.h>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <queue>
#include <unistd.h>
#include <fcntl.h>
#include "util/mmapManager.h"

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

    tundra::mmapManager mmap("test_log_file_12", 1024*1024);

    char buffer[] = "MAIN TEST. MAIN TEST. MAIN TEST\n";
    memcpy(mmap.get(), buffer, sizeof(buffer));

    mmap.sync();

}
