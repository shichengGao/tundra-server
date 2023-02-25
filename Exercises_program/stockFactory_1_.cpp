//
// Created by 郜事成 on 2023/2/7.
//

#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include <functional>
#include <thread>

class Stock {
public:
    Stock(const std::string& key) : key_(key) { }
    const std::string& key() const {
        return key_;
    }
private:
    std::string key_;
};

//对象池
class StockFactory : std::enable_shared_from_this<StockFactory>{
public:
    std::shared_ptr<Stock> get(const std::string& key);

private:
    void weakDeleteCallback(const std::weak_ptr<StockFactory>& wkFactory, Stock* stock);
    void removeStock(Stock* stock);
    mutable std::mutex mtx_;
    std::map<std::string, std::weak_ptr<Stock>> stocks_;
};


std::shared_ptr<Stock> StockFactory::get(const std::string& key) {
    std::shared_ptr<Stock> pStock;
    std::lock_guard<std::mutex> lg(mtx_);
    std::weak_ptr<Stock>& wkStock = stocks_[key];
    pStock = wkStock.lock();

    if (!pStock) {
        pStock.reset(new Stock(key),
                     std::bind(&StockFactory::weakDeleteCallback,
                               this,
                               std::weak_ptr<StockFactory>(shared_from_this()),
                               std::placeholders::_1
                               ));
        wkStock = pStock; // 更新了stocks_[key]，因为wkStock是个引用
    }
    return pStock;
}

void StockFactory::weakDeleteCallback(const std::weak_ptr<StockFactory>& wkFactory, Stock *stock) { //定制的shared_ptr删除器
    std::shared_ptr<StockFactory> factory(wkFactory.lock());
    if( factory ) {
        factory->removeStock(stock);
    }
    delete stock;
}
void StockFactory::removeStock(Stock *stock) {
    if (stock) {
        std::lock_guard<std::mutex> lg(mtx_);
        stocks_.erase(stock->key());
    }
}
