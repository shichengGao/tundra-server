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
        : fName_(filename), len_(len) {

        int fd = -1;
        if (!fName_.empty()) {
            fd = open(fName_.c_str(), O_RDWR);
            assert(fd >= 0);
        }

        addr_ = reinterpret_cast<unsigned char*>(
                mmap( NULL, len, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0 ));

        if (addr_ == MAP_FAILED)
            throw std::runtime_error("mmap failed");

        if (fd > 0)
            close(fd);
    }

    mmapManager::~mmapManager() {
        munmap(addr_, len_);
    }

    unsigned char* mmapManager::get() {
        return addr_;
    }

    void mmapManager::sync() {
        msync(addr_,len_,MS_ASYNC);
    }

} // tundra