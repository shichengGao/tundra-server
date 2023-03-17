//
// Created by scgao on 2023/3/1.
//
#include "String.h"
namespace tundra {

bool inline operator==(const String&  lhs, const String& rhs) {
    return (lhs.size() == rhs.size()) && (memcmp(lhs.data(), rhs.data(), lhs.size()) == 0);
}

bool inline operator!=(const String&lhs, const String& rhs) {
    return !(lhs == rhs);
}

bool operator<(const String&lhs, const String& rhs) {
    int ret = memcmp(lhs.data(), rhs.data(), std::min(lhs.size(), rhs.size()));
    return ret < 0 || (ret == 0 && lhs.size() < rhs.size());
}

bool operator<=(const String& lhs, const String& rhs) {
    return lhs < rhs || lhs == rhs;
}

bool operator>(const String& lhs, const String& rhs) {
    return !(lhs <= rhs);
}

bool operator>=(const String& lhs, const String& rhs) {
    return !(lhs < rhs);
}
}