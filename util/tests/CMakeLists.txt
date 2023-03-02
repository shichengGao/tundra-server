add_executable(Logging_test Logging_test.cpp)
target_link_libraries(Logging_test gtest tundra_util)
add_test(NAME Logging_test COMMAND Logging_test)