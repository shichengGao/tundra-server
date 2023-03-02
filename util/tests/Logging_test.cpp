//
// Created by scgao on 2023/2/27.
//

#include "gtest/gtest.h"
#include "util/Logging.h"

using namespace tundra;

TEST(Logging_general_test, open_file) {
    Logger::instance().open("./test.log");
    Logger::instance().close();
}

TEST(Logging_general_test, format_output) {
    Logger& logger = Logger::instance();
    logger.open("./test.log");
    logger.log(Logger::Level::TRACE,
                           __FILE__, __LINE__,
                           "Hello Log");
    logger.log(Logger::Level::TRACE,
                           __FILE__, __LINE__,
                           "name is %s, age is %d", "jack", 18);
    logger.close();
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}