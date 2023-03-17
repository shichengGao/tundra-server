add_executable(LogFile_test LogFile_test.cpp)
target_link_libraries(LogFile_test gtest tundra_base)
add_test(NAME LogFile_test COMMAND LogFile_test)

add_executable(AsyncLogger_test AsyncLogger_test.cpp)
target_link_libraries(AsyncLogger_test gtest tundra_base)
add_test(NAME AsyncLogger_test COMMAND AsyncLogger_test)