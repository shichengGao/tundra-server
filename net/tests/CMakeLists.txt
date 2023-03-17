add_executable(EventLoop_test EventLoop_test.cpp)
target_link_libraries(EventLoop_test gtest tundra_net)
add_test(NAME EventLoop_test COMMAND EventLoop_test)

add_executable(Acceptor_test Acceptor_test.cpp)
target_link_libraries(Acceptor_test gtest tundra_net)
add_test(NAME Acceptor_test COMMAND Acceptor_test)