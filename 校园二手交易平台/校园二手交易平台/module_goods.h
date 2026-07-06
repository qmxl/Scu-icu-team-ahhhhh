#pragma once
#ifndef MODULE_GOODS_H
#define MODULE_GOODS_H

#include "common.h"
#include "module_user.h"

// ===== 分类类 =====
class Category {
public:
    int id;
    string name;
    Category() : id(0) {}
    Category(int i, string n) : id(i), name(n) {}
};

// ===== 商品类 =====
class Goods {
public:
    int id, userId, categoryId;
    string name, description, price_old, price_new;
    string status; // pending/on_sale/off_sale/sold
    string imagePath;
    int viewCount, collectCount;
    time_t createTime;

    Goods();
    Goods(int i, int uid, int cid, string n, string d, string po, string pn, string s, string img);
};

// ===== 函数声明 =====
void initCategories();
void loadGoods();
void saveGoods();
void loadFavorites();
void saveFavorites();
void publishGoods();
void browseGoods();
void searchGoods();
void viewGoodsDetail(int goodsId);
void toggleFavorite();
void myPublishedGoods();
void editGoods();
void deleteGoods();

#endif
