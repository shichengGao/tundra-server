//
// Created by 郜事成 on 2023/2/10.
//

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <cassert>
using namespace std;
//using Map = map<string, vector<pair<string, int>>>;

class CustomerData {
public:
    CustomerData() : data_(new Map)  { }
    int query(const string& customer, const string& stock) const;

private:
    using Entry = pair<string, int>;
    using EntryList = vector<Entry>;
    using Map = map<string,EntryList>;
    using MapPtr = std::shared_ptr<Map>;

    void update(const string& customer, const EntryList& entries);

    static int findEntry(const EntryList& entries, const string& stock);

    MapPtr getData() const {
        std::lock_guard<std::mutex> lg(mtx_);
        return data_;
    }

    mutable std::mutex mtx_;
    MapPtr data_;
};

int CustomerData::query(const std::string &customer, const std::string &stock) const {
    MapPtr data = getData();
    //拿到数据就不需要锁了,这使得多线程并发读的性能较好
    Map::const_iterator entries = data->find(customer);
    if (entries != data->end())
        return findEntry(entries->second, stock);
    else
        return -1;
}

void CustomerData::update(const std::string &customer, const CustomerData::EntryList &entries) {
    std::lock_guard<std::mutex> lg(mtx_);//写的过程
    if(!data_.unique()) {
        MapPtr newData(new Map(*data_));
        data_.swap(newData);
    }

    assert(data_.unique());
    (*data_)[customer] = entries;
}


