//
// Created by 郜事成 on 2023/2/8.
//

#include <vector>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <cassert>
#include <iostream>

class Foo {
public:
    void doit() const{

    }
};

using FooList = std::vector<Foo>;
using FooListPtr = std::shared_ptr<FooList>;

std::mutex mtx_;
FooListPtr g_foos;

void traverse() {
    FooListPtr foos;
    {
        std::lock_guard<std::mutex> lg(mtx_);
        foos = g_foos;                 //与post配合, 保证临界区外读时g_foos所指的对象不会被析构
        assert(!g_foos.unique());
    }

    //
    for (std::vector<Foo>::const_iterator it = foos->begin(); it != foos->end(); ++it) {
        it->doit();
    }
}

void post(const Foo& f ) {
    std::cout<<"post"<<std::endl;
    std::lock_guard<std::mutex> lg(mtx_);

    if (!g_foos.unique()) {  //当不能修改时，复制一个副本在上面修改
        g_foos.reset(new FooList(*g_foos));
        std::cout<<"copy the whole list"<<std::endl;
    }

    assert(g_foos.unique());
    g_foos->push_back(f);
}
