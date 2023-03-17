//
// Created by scgao on 2023/3/3.
//
#include "base/LogFile.h"
#include "gtest/gtest.h"

using namespace tundra;

TEST(logfile_functions, log) {
    LogFile logfile("LogFile_test_log", LogFile::defaultRollSize);

    char buffer[] = "This is only for TEST, TEST LOG.\n";
    for (int i=0; i<120; ++i) {
        logfile.append(buffer, sizeof(buffer)-1);
    }

    EXPECT_TRUE(true);
}

TEST(logfile_functions, roll) {
    LogFile logFile("LogFile_test_roll", 16 * 1024 * 1024);
    char buffer[] = "012345678\n";

    for (int j=0; j< 1024; ++j) {
        for (int i=0; i<4 * 4096; ++i) {
            logFile.append(buffer, sizeof(buffer)-1);
        }
    }

    logFile.flush();

    EXPECT_TRUE(true);
}

int main(int argc,char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

