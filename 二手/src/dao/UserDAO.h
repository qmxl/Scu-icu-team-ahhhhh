#pragma once

#include <drogon/orm/DbClient.h>
#include "model/User.h"
#include <memory>
#include <string>

class UserDAO {
public:
    static std::shared_ptr<UserDAO> instance();

    // 创建用户
    drogon_model::campus_secondhand::UserPtr createUser(const std::string& studentId,
                                                        const std::string& username,
                                                        const std::string& password,
                                                        const std::string& realName = "",
                                                        const std::string& phone = "",
                                                        const std::string& email = "",
                                                        const std::string& department = "",
                                                        const std::string& grade = "");

    // 根据学号查找用户
    drogon_model::campus_secondhand::UserPtr findByStudentId(const std::string& studentId);

    // 根据ID查找用户
    drogon_model::campus_secondhand::UserPtr findById(int64_t id);

    // 更新用户信息
    bool updateUser(int64_t id, const std::map<std::string, std::string>& fields);

    // 删除用户
    bool deleteUser(int64_t id);

private:
    UserDAO() = default;
    ~UserDAO() = default;
    UserDAO(const UserDAO&) = delete;
    UserDAO& operator=(const UserDAO&) = delete;
};