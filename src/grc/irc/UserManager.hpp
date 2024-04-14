/**
 * @file UserManager.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief UserManager 클래스 정의 헤더 파일.
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
     * @brief User 객체가 존재하는지 확인하는 함수.
     * 
     * @param socket 찾을 User 객체의 소켓.
     * @return true
     * @return false
     */
    static bool IsUserExist(int32 IN socket);
    /**
     * @brief User 객체가 존재하는지 확인하는 함수.
     * 
     * @param nickname 찾을 User 객체의 nickname.
     * @return true
     * @return false
     */
    static bool IsUserExist(const std::string& IN nickname);

    /**
     * @brief 특정 User 객체를 반환하는 함수.
     * 
     * 없는 유저에 대해서 함수를 호출하는 경우, Assertion 발생.
     * 
     * @param socket 찾을 User 객체의 소켓.
     * @return User& : User의 객체를 반환.
     */
    static User& GetUser(const int32 IN socket);
        /**
     * @brief 특정 User 객체를 반환하는 함수.
     * 
     * 없는 유저에 대해서 함수를 호출하는 경우, Assertion 발생.
     * 
     * @param nickname 찾을 User 객체의 nickname.
     * @return User& : User의 객체를 반환.
     */
    static User& GetUser(const std::string& IN nickname);
    /**
     * @brief 현재 모든 User 객체의 목록을 반환하는 함수.
     * 
     * @return const std::map<int32, User>& : 멤버 변수 sStaticUsers 반환.
     */
    static const std::map<int32, User>& GetUsers();

    /**
     * @brief User 객체를 추가하는 함수.
     * 
     * @param socket 추가할 User 객체의 소켓.
     */
    static void AddUser(const int32 IN socket);
    /**
     * @brief User 객체를 삭제하는 함수.
     * 
     * @param socket 삭제할 User 객체의 소켓.
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
     * @brief 현재 IRC에 연결된 모든 User 객체의 목록.
     * 
     */
    static std::map<int32, User> sStaticUsers;
};

}
