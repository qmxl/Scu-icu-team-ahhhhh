#include "cache/CacheManager.h"

namespace cache {

CacheManager& CacheManager::getInstance() {
    static CacheManager instance;
    return instance;
}

void CacheManager::setUser(const model::User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    users_[user.id] = user;
}

bool CacheManager::hasUser(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    return users_.find(id) != users_.end();
}

model::User CacheManager::getUser(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = users_.find(id);
    return it != users_.end() ? it->second : model::User();
}

void CacheManager::removeUser(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    users_.erase(id);
}

void CacheManager::setGoods(const model::Goods& goods) {
    std::lock_guard<std::mutex> lock(mutex_);
    goods_[goods.id] = goods;
}

bool CacheManager::hasGoods(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    return goods_.find(id) != goods_.end();
}

model::Goods CacheManager::getGoods(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = goods_.find(id);
    return it != goods_.end() ? it->second : model::Goods();
}

void CacheManager::removeGoods(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    goods_.erase(id);
}

std::vector<model::Goods> CacheManager::getAllGoods() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<model::Goods> result;
    for (auto& pair : goods_) {
        result.push_back(pair.second);
    }
    return result;
}

void CacheManager::setTransaction(const model::Transaction& transaction) {
    std::lock_guard<std::mutex> lock(mutex_);
    transactions_[transaction.id] = transaction;
}

bool CacheManager::hasTransaction(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    return transactions_.find(id) != transactions_.end();
}

model::Transaction CacheManager::getTransaction(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = transactions_.find(id);
    return it != transactions_.end() ? it->second : model::Transaction();
}

void CacheManager::removeTransaction(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    transactions_.erase(id);
}

std::vector<model::Transaction> CacheManager::getAllTransactions() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<model::Transaction> result;
    for (auto& pair : transactions_) {
        result.push_back(pair.second);
    }
    return result;
}

}