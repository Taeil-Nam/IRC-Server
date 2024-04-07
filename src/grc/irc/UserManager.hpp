/**
 * @file UserManager.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief User 관리용 정적 클래스
 * @version 0.1
 * @date 2024-04-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "common.hpp"
#include "User.hpp"

namespace grc
{

class UserManager
{
public:
    static bool IsUserExist(int32 IN socket);
    static bool IsUserExist(const std::string& IN nickname);

    static User& GetUser(const int32 IN socket);
    static User& GetUser(const std::string& IN nickname);
    static const std::map<int32, User>& GetUsers();

    static void AddUser(const int32 IN socket);
    static void DeleteUser(const int32 IN socket);
private:
    UserManager(); // = delete

private:
    static std::map<int32, User> sStaticUsers;
};

}
