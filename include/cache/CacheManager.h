#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include "model/User.h"
#include "model/Goods.h"
#include "model/Transaction.h"
#include <unordered_map>
#include <vector>
#include <mutex>

namespace cache {

class CacheManager {
public:
    static CacheManager& getInstance();
    
    void setUser(const model::User& user);
    bool hasUser(int id);
    model::User getUser(int id);
    void removeUser(int id);
    
    void setGoods(const model::Goods& goods);
    bool hasGoods(int id);
    model::Goods getGoods(int id);
    void removeGoods(int id);
    std::vector<model::Goods> getAllGoods();
    
    void setTransaction(const model::Transaction& transaction);
    bool hasTransaction(const std::string& id);
    model::Transaction getTransaction(const std::string& id);
    void removeTransaction(const std::string& id);
    std::vector<model::Transaction> getAllTransactions();
    
private:
    CacheManager() = default;
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;
    
    std::unordered_map<int, model::User> users_;
    std::unordered_map<int, model::Goods> goods_;
    std::unordered_map<std::string, model::Transaction> transactions_;
    
    std::mutex mutex_;
};

}

#endif