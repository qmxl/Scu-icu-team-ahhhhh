#pragma once

#include <drogon/HttpController.h>
#include "dao/UserDAO.h"
#include "utils/JwtUtil.h"
#include "utils/PasswordUtil.h"

using namespace drogon;

namespace api
{
namespace v1
{
class UserController : public drogon::HttpController<UserController>
{
public:
    METHOD_LIST_BEGIN
    METHOD_ADD(UserController::registerUser, "/api/v1/user/register", Post, Options);
    METHOD_ADD(UserController::login, "/api/v1/user/login", Post, Options);
    METHOD_ADD(UserController::getProfile, "/api/v1/user/profile", Get, Options);
    METHOD_ADD(UserController::updateProfile, "/api/v1/user/profile", Put, Options);
    METHOD_ADD(UserController::deleteUser, "/api/v1/user", Delete, Options);
    METHOD_LIST_END

    // 注册
    void registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 登录
    void login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取用户信息
    void getProfile(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 更新用户信息
    void updateProfile(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 删除用户
    void deleteUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};
}
}