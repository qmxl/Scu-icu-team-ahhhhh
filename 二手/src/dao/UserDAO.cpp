#include "UserDAO.h"
#include <drogon/drogon.h>

std::shared_ptr<UserDAO> UserDAO::instance() {
    static std::shared_ptr<UserDAO> instance = std::make_shared<UserDAO>();
    return instance;
}

drogon_model::campus_secondhand::UserPtr UserDAO::createUser(const std::string& studentId,
                                                             const std::string& username,
                                                             const std::string& password,
                                                             const std::string& realName,
                                                             const std::string& phone,
                                                             const std::string& email,
                                                             const std::string& department,
                                                             const std::string& grade) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        drogon_model::campus_secondhand::User user;
        user.setStudentId(studentId);
        user.setUsername(username);
        user.setPassword(password);
        user.setRealName(realName);
        user.setPhone(phone);
        user.setEmail(email);
        user.setDepartment(department);
        user.setGrade(grade);
        user.setStatus(1);

        auto result = client->insert(user);
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "创建用户失败: " << e.what();
        return nullptr;
    }
}

drogon_model::campus_secondhand::UserPtr UserDAO::findByStudentId(const std::string& studentId) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        auto result = client->findFirst<drogon_model::campus_secondhand::User>(
            drogon::orm::Criteria("student_id", drogon::orm::CompareOperator::EQ, studentId)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "查找用户失败: " << e.what();
        return nullptr;
    }
}

drogon_model::campus_secondhand::UserPtr UserDAO::findById(int64_t id) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        auto result = client->findFirst<drogon_model::campus_secondhand::User>(
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "查找用户失败: " << e.what();
        return nullptr;
    }
}

bool UserDAO::updateUser(int64_t id, const std::map<std::string, std::string>& fields) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        drogon::orm::Update update;
        for (const auto& [key, value] : fields) {
            update.set(key, value);
        }

        auto result = client->update<drogon_model::campus_secondhand::User>(
            update,
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "更新用户失败: " << e.what();
        return false;
    }
}

bool UserDAO::deleteUser(int64_t id) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        auto result = client->deleteRows<drogon_model::campus_secondhand::User>(
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "删除用户失败: " << e.what();
        return false;
    }
}