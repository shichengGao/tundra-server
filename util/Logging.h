//
// Created by scgao on 2023/2/23.
//

#ifndef TUNDRA_LOGGING_H
#define TUNDRA_LOGGING_H

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


#endif //TUNDRA_LOGGING_H
