add_executable(LogFile_test LogFile_test.cpp)
target_link_libraries(LogFile_test gtest tundra_base)
add_test(NAME LogFile_test COMMAND LogFile_test)