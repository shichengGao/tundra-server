//
// Created by scgao on 2023/2/25.
//

#ifndef TUNDRA_MMAPMANAGER_H
#define TUNDRA_MMAPMANAGER_H

#include <unistd.h>
#include <string>

namespace tundra {

class mmapManager {
public:
    mmapManager(const mmapManager&) = delete;
    mmapManager operator=(const mmapManager&) = delete;

    explicit mmapManager(int len);
    explicit mmapManager(const std::string& filename, int len);
    ~mmapManager();

    void append(const char* buf, int len);

    const char* data() const {
        return reinterpret_cast<const char*>(addr_);
    }

    int len() const;
    void sync();

private:
    const std::string fName_;
    unsigned char* addr_;
    int fd_;
    int curPos_;
    const int len_;
};

} // tundra

#endif //TUNDRA_MMAPMANAGER_H
