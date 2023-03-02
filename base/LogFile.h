//
// Created by scgao on 2023/3/2.
//

#ifndef TUNDRA_LOGFILE_H
#define TUNDRA_LOGFILE_H

#include <memory>
#include "util/mmapManager.h"

namespace tundra {

class LogFile {
public:
    LogFile(const std::string& basename, off_t rollSize)
        : basename_(basename), rollSize_(rollSize)
    {


    }

private:
    std::string basename_;
    off_t rollSize_;
    std::unique_ptr<mmapManager> curMMap_;

};

}



#endif //TUNDRA_LOGFILE_H
