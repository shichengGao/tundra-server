//
// Created by scgao on 2023/3/3.
//

#include "base/AsyncLogger.h"
#include "gtest/gtest.h"

using namespace tundra;

TEST(AsyncLogger_function_test, single_thread_log) {
    AsyncLogger logger("async_func_test_single_thread_log", LogFile::defaultRollSize);
    logger.start();

    char buf1[] = "THE MESSAGE ONE. ONE. ONE";
    char buf2[] = "THE MESSAGE TWO. TWO. TWO";
    char buf3[] = "THE MESSAGE THR. THR. THR";
    size_t size = sizeof(buf1) - 1;

    std::vector<const char*> bufs = { buf1, buf2, buf3};
    for (int i=0; i< 1024 ; ++i) {
        for (int j=0; j < 1024; ++j) {
            int k = rand() % 3;
            logger.append(bufs[k], size, tundra::AsyncLogger::DEBUG);
        }
        usleep(300);
    }
    logger.stop();
    EXPECT_TRUE(true);
    system("rm async_func_test_single_thread_log*.log");
}

TEST(AsyncLogger_function_test, single_thread_roll) {
    AsyncLogger logger("async_func_test_single_thread_roll", LogFile::defaultRollSize/4);
    logger.start();

    char buf1[] = "THE MESSAGE ONE. ONE. ONE";
    char buf2[] = "THE MESSAGE TWO. TWO. TWO";
    char buf3[] = "THE MESSAGE THR. THR. THR";
    size_t size = sizeof(buf1) - 1;

    std::vector<const char*> bufs = { buf1, buf2, buf3};

    for (int i=0; i<1024 ; ++i) {
        for (int j=0; j < 2048; ++j) {
            int k = rand() % 3;
            logger.append(bufs[k], size, tundra::AsyncLogger::DEBUG);
        }
        usleep(300);
    }

    logger.stop();
    EXPECT_TRUE(true);
    system("rm async_func_test_single_thread_roll*.log");
}


TEST(AsyncLogger_function_test, multi_thread_log) {
    AsyncLogger logger("async_func_test_multi_thread_log", LogFile::defaultRollSize);
    logger.start();

    auto logFunc = [&](int id){
        char buf[32];
        snprintf(buf, sizeof(buf), " message from thread %d", id);
        size_t size = sizeof(buf) - 1;


        for (int i=0; i<1024 ; ++i) {
            for (int j=0; j < 1024; ++j) {
                logger.append(buf, size, tundra::AsyncLogger::DEBUG);
            }
            usleep(300);
        }
    };

    std::vector<std::thread> threads_;
    for (int i=0; i<8; ++i) {
        threads_.emplace_back(std::thread(logFunc, i));
    }

    for (auto& t : threads_) {
        t.join();
    }

    logger.stop();

    EXPECT_TRUE(true);
    system("rm async_func_test_multi_thread_log*.log");
}

/*
 *  Be close to 1000,000 logs per seconds with single thread.
 */

TEST(AsyncLogger_benchmark, bench) {
    AsyncLogger logger("async_benchmark", LogFile::defaultRollSize);
    logger.start();

    int cnt = 0;
    const int kBatch = 1000*1000;
    const int rounds = 5;
    std::string empty = " ";
    std::string longStr(3000, 'x');
    longStr += " ";

    double totalTime = 0.0;


    for (int t = 0; t < rounds; ++t) {
        TimeStamp start = TimeStamp::now();
        for (int i=0; i< kBatch; ++i) {
            char buf[128];
            int len = snprintf(buf, sizeof(buf),
                               "Hello 0123456789 abcdefghijklmnopqrstuvwxyz %s %d",
                               i % 10 == 1 ? longStr.c_str() : empty.c_str(), cnt++);

            logger.append(buf, len, tundra::AsyncLogger::DEBUG);

        }
        TimeStamp end = TimeStamp::now();
        totalTime += timeDiff(start, end);
    }

    logger.stop();

    printf("time consumption : %f s per %d logs \n", totalTime / rounds, kBatch);

    system("rm async_benchmark*.log");

}

/*
 *
 */
TEST(AsyncLogger_benchmark, bench_multi_thread) {
    AsyncLogger logger("async_benchmark_multi_thread", LogFile::defaultRollSize);
    logger.start();

    auto threadFunc = [&logger](int id) {
        int cnt = 0;
        const int kBatch = 1000000;
        const int rounds = 5;
        std::string empty = " ";
        std::string longStr(3000, 'x');
        longStr += " ";

        double totalTime = 0.0;

        for (int t = 0; t < rounds; ++t) {
            TimeStamp start = TimeStamp::now();
            for (int i=0; i< kBatch; ++i) {
                char buf[128];
                int len = snprintf(buf, sizeof(buf),
                                   "Hello 0123456789 abcdefghijklmnopqrstuvwxyz %s %d , id : %d",
                                   i % 10 == 0 ? longStr.c_str() : empty.c_str(), cnt++, id);

                logger.append(buf, len, tundra::AsyncLogger::DEBUG);

            }
            TimeStamp end = TimeStamp::now();

            totalTime += timeDiff(start, end);
        }
        printf("thread %d time consumption : %f s per %d logs \n", id, totalTime / rounds, kBatch);
    };


    std::vector<std::thread> threads_;
    TimeStamp start = TimeStamp::now();
    int threadNum = 8;
    for (int i=0; i<threadNum; ++i) {
        threads_.emplace_back(std::thread(threadFunc, i));
    }

    for (auto&t : threads_)
        t.join();
    TimeStamp end = TimeStamp::now();
    logger.stop();

    printf("totalTime : %f s for %d logs.", timeDiff(start,end), threadNum * 1000000 * 5);

    system("rm async_benchmark_multi_thread*.log");

}


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
