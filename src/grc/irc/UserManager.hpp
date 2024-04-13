/**
 * @file UserManager.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief UserManager 클래스를 정의한다.
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

/**
 * @class UserManager
 * @brief User 객체들을 관리하는 정적 클래스.
 * 
 */
class UserManager
{
public:
    /**
     * @brief 유저가 존재하는지 확인하는 함수.
     * 
     * @param socket 찾을 유저의 소켓.
     * @return true 유저가 존재 하는 경우 true 반환.
     * @return false 유저가 존재 하지 않는 경우 false 반환.
     */
    static bool IsUserExist(int32 IN socket);
    /**
     * @brief 유저가 존재하는지 확인하는 함수.
     * 
     * @param nickname 찾을 유저의 nickname.
     * @return true 유저가 존재 하는 경우 true 반환.
     * @return false 유저가 존재 하지 않는 경우 false 반환.
     */
    static bool IsUserExist(const std::string& IN nickname);

    /**
     * @brief 특정 User 객체를 반환하는 함수.
     * 
     * @param socket 찾을 유저 객체의 소켓.
     * @return User& 유저의 객체를 반환.
     */
    static User& GetUser(const int32 IN socket);
        /**
     * @brief 특정 User 객체를 반환하는 함수.
     * 
     * @param nickname 찾을 유저 객체의 nickname.
     * @return User& 유저의 객체를 반환.
     */
    static User& GetUser(const std::string& IN nickname);
    /**
     * @brief 현재 모든 유저 목록을 반환하는 함수.
     * 
     * @return const std::map<int32, User>& 멤버 변수 sStaticUsers 반환.
     */
    static const std::map<int32, User>& GetUsers();

    /**
     * @brief 유저를 추가하는 함수.
     * 
     * @param socket 추가할 유저의 소켓.
     */
    static void AddUser(const int32 IN socket);
    /**
     * @brief 유저를 삭제하는 함수.
     * 
     * @param socket 삭제할 유저의 소켓.
     */
    static void DeleteUser(const int32 IN socket);
private:
    /**
     * @brief UserManager 객체의 기본 생성자. (사용되지 않음)
     * 
     */
    UserManager(); // = delete

private:
    /**
     * @brief 현재 IRC에 연결된 모든 유저 목록.
     * 
     */
    static std::map<int32, User> sStaticUsers;
};

}
