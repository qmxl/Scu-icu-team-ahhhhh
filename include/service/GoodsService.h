#ifndef GOODS_SERVICE_H
#define GOODS_SERVICE_H

#include "model/Goods.h"
#include <string>
#include <vector>

namespace service {

class GoodsService {
public:
    static GoodsService& getInstance();
    
    bool publishGoods(const model::Goods& goods);
    std::vector<model::Goods> getGoodsList(int page, int pageSize, const std::string& category = "", 
                                            const std::string& keyword = "", int sellerId = 0);
    model::Goods getGoodsById(int id);
    bool updateGoods(const model::Goods& goods);
    bool deleteGoods(int id);
    bool setGoodsStatus(int id, int status);
    bool toggleLock(int id, bool isLocked);
    int getGoodsCount();
    int getGoodsCountByCategory(const std::string& category);
    int getGoodsCountBySeller(int sellerId);
    
private:
    GoodsService() = default;
    GoodsService(const GoodsService&) = delete;
    GoodsService& operator=(const GoodsService&) = delete;
};

}

#endif