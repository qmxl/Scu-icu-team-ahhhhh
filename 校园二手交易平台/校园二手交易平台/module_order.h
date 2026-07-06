#pragma once
#ifndef MODULE_ORDER_H
#define MODULE_ORDER_H

#include "common.h"
#include "module_user.h"
#include "module_goods.h"

// ===== 订单类 =====
class Order {
public:
    int id, buyerId, sellerId, goodsId;
    double amount;
    string status; // pending/paid/shipped/received/cancelled
    string address;
    time_t createTime, payTime;

    Order() : id(0), buyerId(0), sellerId(0), goodsId(0), amount(0) {}
};

// ===== 地址类 =====
class Address {
public:
    int id, userId;
    string receiver, phone, province, city, district, detail;
    bool isDefault;

    Address() : id(0), userId(0), isDefault(false) {}
};

// ===== 函数声明 =====
void loadOrders();
void saveOrders();
void loadAddresses();
void saveAddresses();
void createOrder();
void payOrder();
void myOrders();
void manageAddress();

#endif
