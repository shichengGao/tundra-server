//
// Created by 郜事成 on 2023/2/23.
//

#ifndef MUDUOTRAIN_LOGGING_H
#define MUDUOTRAIN_LOGGING_H

namespace tundra{

class Logger {
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

};

#define LOG
}


#endif //MUDUOTRAIN_LOGGING_H
