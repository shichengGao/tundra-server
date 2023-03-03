//
// Created by scgao on 2023/3/3.
//
#include "base/LogFile.h"
#include "gtest/gtest.h"

using namespace tundra;

TEST(logfile_functions, log) {
    LogFile logfile("test_log_func", LogFile::defaultRollSize);

    char buffer[] = "This is only for TEST, TEST LOG.\r\n";

    for (int i=0; i<120; ++i) {
        logfile.append(buffer, sizeof(buffer));
    }
    logfile.flush();

    EXPECT_TRUE(true);
}

int main(int argc,char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

