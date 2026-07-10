#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "model/User.h"
#include <string>
#include <vector>

namespace service {

class UserService {
public:
    static UserService& getInstance();
    
    model::User login(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& password,
                      const std::string& phone, const std::string& qq);
    model::User getUserById(int id);
    model::User getUserByUsername(const std::string& username);
    bool updateProfile(int id, const std::string& phone, const std::string& qq);
    std::vector<model::User> getAllUsers(int page, int pageSize);
    bool updateUserStatus(int id, int status);
    int getUserCount();
    
private:
    UserService() = default;
    UserService(const UserService&) = delete;
    UserService& operator=(const UserService&) = delete;
    
    std::string hashPassword(const std::string& password);
};

}

#endif