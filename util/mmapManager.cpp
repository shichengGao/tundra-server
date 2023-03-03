//
// Created by scgao on 2023/2/25.
//

#include "mmapManager.h"
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdexcept>

namespace tundra {
    mmapManager::mmapManager(int len)
        : mmapManager("", len){

    }

    mmapManager::mmapManager(const std::string& filename, int len)
        : fd_(-1), fName_(filename), len_(len) {

        if (!fName_.empty()) {
            fd_ = open(fName_.c_str(), O_CREAT|O_APPEND|O_RDWR);
            assert(fd_ >= 0);
        }

        //FIXME: wsl bug, move tundra to native linux afterwards
        lseek(fd_,len, SEEK_SET);
        write(fd_, " ", 1);;


        addr_ = reinterpret_cast<unsigned char*>(
                mmap(NULL, len, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd_, 0 ));

        if (addr_ == MAP_FAILED) {
            perror("mmap failed");
        }

    }

    mmapManager::~mmapManager() {
        munmap(addr_, len_);
        if (fd_ >= 0)
            close(fd_);
    }

    unsigned char* mmapManager::get() {
        return addr_;
    }

    void mmapManager::sync() {
        if (msync(addr_,len_,MS_ASYNC) == -1)
            perror("mmap sync failed.");
    }

} // tundra