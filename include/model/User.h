#ifndef USER_H
#define USER_H

#include <string>

namespace model {

struct User {
    int id = 0;
    std::string username;
    std::string password;
    std::string phone;
    std::string qq;
    int role = 0;
    int status = 1;
    std::string created_at;
    std::string updated_at;
};

}

#endif