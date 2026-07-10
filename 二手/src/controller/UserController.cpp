#include "UserController.h"

namespace api
{
namespace v1
{

void UserController::registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        auto json = req->getJsonObject();
        if (!json) {
            response["code"] = 400;
            response["message"] = "请求参数错误";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        std::string studentId = (*json)["student_id"].asString();
        std::string username = (*json)["username"].asString();
        std::string password = (*json)["password"].asString();
        std::string phone = (*json)["phone"].asString();

        if (studentId.empty() || username.empty() || password.empty()) {
            response["code"] = 400;
            response["message"] = "学号、用户名和密码不能为空";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 检查学号是否已存在
        auto existingUser = UserDAO::instance()->findByStudentId(studentId);
        if (existingUser) {
            response["code"] = 400;
            response["message"] = "该学号已被注册";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 创建用户
        std::string hashedPassword = PasswordUtil::hashPassword(password);
        auto user = UserDAO::instance()->createUser(studentId, username, hashedPassword, phone);

        if (user) {
            response["code"] = 200;
            response["message"] = "注册成功";
            response["data"]["id"] = user->getId();
            response["data"]["student_id"] = user->getStudentId();
            response["data"]["username"] = user->getUsername();
        } else {
            response["code"] = 500;
            response["message"] = "注册失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "注册异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void UserController::login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        auto json = req->getJsonObject();
        if (!json) {
            response["code"] = 400;
            response["message"] = "请求参数错误";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        std::string studentId = (*json)["student_id"].asString();
        std::string password = (*json)["password"].asString();

        if (studentId.empty() || password.empty()) {
            response["code"] = 400;
            response["message"] = "学号和密码不能为空";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 查找用户
        auto user = UserDAO::instance()->findByStudentId(studentId);
        if (!user) {
            response["code"] = 401;
            response["message"] = "学号或密码错误";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 验证密码
        if (!PasswordUtil::verifyPassword(password, user->getPassword())) {
            response["code"] = 401;
            response["message"] = "学号或密码错误";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 生成JWT Token
        std::string token = JwtUtil::instance()->generateToken(user->getId());

        response["code"] = 200;
        response["message"] = "登录成功";
        response["data"]["token"] = token;
        response["data"]["user"]["id"] = user->getId();
        response["data"]["user"]["student_id"] = user->getStudentId();
        response["data"]["user"]["username"] = user->getUsername();
    } catch (const std::exception& e) {
        LOG_ERROR << "登录异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void UserController::getProfile(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        auto user = UserDAO::instance()->findById(userId);
        if (!user) {
            response["code"] = 404;
            response["message"] = "用户不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        response["code"] = 200;
        response["message"] = "success";
        response["data"]["id"] = user->getId();
        response["data"]["student_id"] = user->getStudentId();
        response["data"]["username"] = user->getUsername();
        response["data"]["phone"] = user->getPhone();
        response["data"]["created_at"] = user->getCreatedAt().toDbString();
    } catch (const std::exception& e) {
        LOG_ERROR << "获取用户信息异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void UserController::updateProfile(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        auto json = req->getJsonObject();
        if (!json) {
            response["code"] = 400;
            response["message"] = "请求参数错误";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        std::map<std::string, std::string> fields;
        if ((*json).isMember("username") && !(*json)["username"].asString().empty()) {
            fields["username"] = (*json)["username"].asString();
        }
        if ((*json).isMember("phone") && !(*json)["phone"].asString().empty()) {
            fields["phone"] = (*json)["phone"].asString();
        }

        bool success = UserDAO::instance()->updateUser(userId, fields);

        if (success) {
            response["code"] = 200;
            response["message"] = "更新成功";
        } else {
            response["code"] = 500;
            response["message"] = "更新失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "更新用户信息异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void UserController::deleteUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        bool success = UserDAO::instance()->deleteUser(userId);

        if (success) {
            response["code"] = 200;
            response["message"] = "删除成功";
        } else {
            response["code"] = 500;
            response["message"] = "删除失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "删除用户异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

}
}