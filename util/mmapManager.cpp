//
// Created by scgao on 2023/2/25.
//

#include "mmapManager.h"
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>

namespace tundra {
    mmapManager::mmapManager(int len)
        : mmapManager("", len){

    }

    mmapManager::mmapManager(const std::string& filename, int len)
        : fd_(-1), curPos_(0), fName_(filename), len_(len){

        if (!fName_.empty()) {
            fd_ = open(fName_.c_str(), O_CREAT|O_TRUNC|O_RDWR, 0644);
            assert(fd_ >= 0);
        }

        //FIXME: wsl bug, move tundra to native linux afterwards
        if(lseek(fd_,len-1, SEEK_SET) == -1) {
            perror("lseek failed.\n");
        }

        if (write(fd_, "", 1) == -1) {
            perror("Error writing last byte of file\n");
        }


        addr_ = reinterpret_cast<unsigned char*>(
                mmap(NULL, len, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd_, 0 ));

        if (addr_ == MAP_FAILED) {
            perror("mmap failed");
        }

        close(fd_);

    }

    mmapManager::~mmapManager() {
        if (munmap(addr_, len_) == -1) {
            perror("munmap failed.\n");
        }
        truncate(fName_.c_str(), curPos_);
        close(fd_);
    }

    void mmapManager::append(const char *buf, int len) {
        assert(curPos_ + len <= this->len_);
        std::memcpy((addr_ + curPos_), buf, len);
        curPos_ += len;
    }

    void mmapManager::sync() {
        if (msync(addr_,len_,MS_ASYNC) == -1)
            perror("mmap sync failed.");
    }

} // tundra