#include <iostream>
#include <set>
#include <memory>

int main() {
    std::set<std::string> mySet = {"foo", "bar", "baz"};

    // 移动元素 "foo" 到另一个 set
    auto it = mySet.find("foo");
    if (it != mySet.end()) {
        std::unique_ptr<std::string> ptr(new std::string(std::move(*it)));
        mySet.erase(it);
        std::set<std::unique_ptr<std::string>> otherSet;
        otherSet.insert(std::move(ptr));
    }

    // 输出两个 set 中的元素
    std::cout << "mySet: ";
    for (const auto& elem : mySet) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    std::cout << "otherSet: ";
    for (const auto& elem : otherSet) {
        std::cout << *elem << " ";
    }
    std::cout << std::endl;

    return 0;
}
