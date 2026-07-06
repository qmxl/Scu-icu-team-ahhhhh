#pragma once
#ifndef MODULE_USER_H
#define MODULE_USER_H

#include "common.h"

// ===== 用户类 =====
class User {
public:
    int id;
    string username, password, nickname, phone, email, role;
    bool isActive;

    User() : id(0), isActive(true) {}
    User(int i, string un, string pw, string nn, string ph, string em, string r);
};

// ===== 函数声明 =====
void loadUsers();
void saveUsers();
bool registerUser();
bool loginUser();
void logoutUser();
void modifyProfile();
bool findPassword();
User* getCurrentUser();
bool isAdmin();

#endif
