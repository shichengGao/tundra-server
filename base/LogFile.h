//
// Created by scgao on 2023/3/2.
//

#ifndef TUNDRA_LOGFILE_H
#define TUNDRA_LOGFILE_H

#include <memory>
#include <assert.h>
#include "util/mmapManager.h"

namespace tundra {

class LogFile {
public:
    LogFile(const std::string& basename, off_t rollSize)
        : basename_(basename), rollSize_(rollSize),cur_bytes_(0)
    {
        assert(basename.find('/') == std::string::npos);
        rollFile();
    }

    void append(const char* logMsgs, int len);

    void flush() {
        curMMap_->sync();
    }

    void rollFile();

    static const off_t defaultRollSize = 128 * 1024 * 1024; //128M

private:
    static std::string getLogFileName(const std::string& basename, time_t* now);
    std::string basename_;
    const off_t rollSize_;
    off_t cur_bytes_;
    std::unique_ptr<mmapManager> curMMap_;

};

}



#endif //TUNDRA_LOGFILE_H
