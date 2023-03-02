//
// Created by scgao on 2023/2/27.
//

#include "Logging.h"
#include "TimeStamp.h"

namespace tundra {

//const char* const Logger::levels_[] = {
//            "TRACE",
//            "DEBUG",
//            "INFO",
//            "WARN",
//            "ERROR",
//            "FATAL",
//        };

const String Logger::levels_[] = {
        "TRACE",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
};

Logger::Logger() {
//    levels_[0] = String("TRACE");
//    levels_[1] = String("DEBUG");
//    levels_[2] = String("INFO");
//    levels_[3] = String("WARN");
//    levels_[4] = String("ERROR");
//    levels_[5] = String("FATAL");
}

Logger::~Logger() {
    close();
}

void Logger::open(const std::string& filename) {
    filename_ = filename;

    fout_.open(filename, std::ios::app);
    if (fout_.fail()) {
        throw std::runtime_error("open file faild.");
    }
}

void Logger::close() {
    fout_.close();
}

void Logger::log(tundra::Logger::Level level, const char *file, int line, const char *format, ...) {
    if (fout_.fail()) {
        throw std::runtime_error("open file failed.");
    }

    assert(level != tundra::Logger::LEVEL_COUNT); //valid log level

    String time = TimeStamp::now().toFormatString(true);

    const char* fmt = "%s %s %s:%d ";
    int size = snprintf(NULL, 0, fmt, time.data(), levels_[level].data(), file, line);
    if (size > 0) {
        char* buffer = new char[size+1];
        snprintf(buffer, size + 1, fmt, time.data(), levels_[level].data(), file, line);
        buffer[size] = 0;
        fout_ << buffer << std::endl;
        delete buffer;
    }
}

}
