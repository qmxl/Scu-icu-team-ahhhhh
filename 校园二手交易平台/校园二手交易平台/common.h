#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <conio.h>
using namespace std;

// ===== 全局变量声明 =====
extern vector<class User> users;
extern vector<class Goods> goods;
extern vector<class Category> categories;
extern vector<class Order> orders;
extern vector<class Comment> comments;
extern vector<class Feedback> feedbacks;
extern vector<class Address> addresses;
extern vector<int> favorites;
extern int currentUserId;

// ===== 工具函数 =====
void clearScreen();
void pauseScreen();
string getCurrentTime();

#endif
