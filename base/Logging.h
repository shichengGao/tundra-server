//
// Created by scgao on 2023/3/4.
//

#ifndef TUNDRA_LOGGING_H
#define TUNDRA_LOGGING_H
#include "base/AsyncLogger.h"
namespace tundra{

class Logging {
private:
    Logging();
    ~Logging();
    Logging(const Logging&) = delete;
    Logging operator=(const Logging&) = delete;

public:
    //TODO: Ensure initialization sequence
    static AsyncLogger& instance();

    static void InitLoggingConfig(const std::string& basename,
                off_t rollSize, int flushInterval);

    static std::string basename_;
    static off_t rollSize_;
    static int flushInterval_;

};

}



#endif //TUNDRA_LOGGING_H
