/**
 * @file User.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief User 클래스를 정의한다.
 * @version 0.1
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../../common.hpp"

using namespace gdf;

namespace grc
{

/**
 * @class User 
 * @brief IRC에 연결되는 유저 객체를 정의하는 클래스.
 */
class User
{
public:
    /**
     * @brief User 객체의 기본 생성자.
     * 
     * 객체가 생성될 때, 모든 멤버 변수들을 초기화한다.
     */
    User();
    /**
     * @brief User 객체의 복사 대입 연산자.
     * 
     * 현재 객체에 매개 변수로 받은 User의 값을 전부 복사한다.
     * 
     * @param User 복사할 User 객체.
     * @return const User& 복사된 User 객체.
     */
    const User& operator=(const User& IN User);
    /**
     * @brief User 객체의 소멸자.
     */
    virtual ~User();

    /**
     * @brief User가 인증 되었는지 확인하는 함수.
     * 
     * @return true 인증되어 있는 경우 true 반환.
     * @return false 인증되어 있지 않은 경우 false 반환.
     */
    bool IsAuthenticated() const;
    /**
     * @brief User가 등록 되었는지 확인하는 함수.
     * 
     * @return true 등록되어 있는 경우 true 반환.
     * @return false 등록되어 있지 않은 경우 false 반환.
     */
    bool IsRegistered() const;

    /**
     * @brief User 객체의 소켓을 반환하는 함수.
     * 
     * @return int32 User 객체의 소켓.
     */
    int32 GetSocket() const;
    /**
     * @brief User 객체의 nickname을 반환하는 함수.
     * 
     * @return const std::string& User 객체의 nickname.
     */
    const std::string& GetNickname() const;
    /**
     * @brief User 객체의 username을 반환하는 함수.
     * 
     * @return const std::string& User 객체의 username.
     */
    const std::string& GetUsername() const;
    /**
     * @brief User 객체의 hostname을 반환하는 함수.
     * 
     * @return const std::string& User 객체의 hostname.
     */
    const std::string& GetHostname() const;
    /**
     * @brief User 객체의 servername을 반환하는 함수.
     * 
     * @return const std::string& User 객체의 servername.
     */
    const std::string& GetServername() const;
    /**
     * @brief User 객체의 realname을 반환하는 함수.
     * 
     * @return const std::string& User 객체의 realname.
     */
    const std::string& GetRealname() const;

    /**
     * @brief User 객체의 소켓을 설정하는 함수.
     * 
     * @param socket target socket.
     */
    void SetSocket(const int32 IN socket);
    /**
     * @brief User 객체의 인증 상태를 true로 설정하는 함수.
     */
    void SetAuthenticated();
    /**
     * @brief User 객체의 등록 상태를 true로 설정하는 함수.
     */
    void SetRegistered();
    /**
     * @brief User 객체의 nickname을 설정하는 함수.
     * 
     * @param nickname target nickname.
     */
    void SetNickname(const std::string& IN nickname);
    /**
     * @brief User 객체의 username을 설정하는 함수.
     * 
     * @param username target username.
     */
    void SetUsername(const std::string& IN username);
    /**
     * @brief User 객체의 hostname을 설정하는 함수.
     * 
     * @param hostname target hostname.
     */
    void SetHostname(const std::string& IN hostname);
    /**
     * @brief User 객체의 servername을 설정하는 함수.
     * 
     * @param servername target servername.
     */
    void SetServername(const std::string& IN servername);
    /**
     * @brief User 객체의 realname을 설정하는 함수.
     * 
     * @param realname target realname.
     */
    void SetRealname(const std::string& IN realname);
private:
    /**
     * @brief User 객체의 복사 생성자. (사용되지 않음)
     * 
     * @param User 복사할 User 객체.
     */
    User(const User& IN User); // = delete

private:
    /**
     * @brief User의 소켓을 저장하는 멤버 변수.
     */
    int32 mSocket;
    /**
     * @brief User의 인증 상태를 저장하는 멤버 변수.
     */
    bool mbIsAuthenticated;
    /**
     * @brief User의 등록 상태를 저장하는 멤버 변수.
     */
    bool mbIsRegistered;
    /**
     * @brief User의 nickname을 저장하는 멤버 변수.
     */
    std::string mNickname;
    /**
     * @brief User의 username을 저장하는 멤버 변수.
     */
    std::string mUsername;
    /**
     * @brief User의 hostname을 저장하는 멤버 변수.
     */
    std::string mHostname;
    /**
     * @brief User의 servername을 저장하는 멤버 변수.
     */
    std::string mServername;
    /**
     * @brief User의 real을 저장하는 멤버 변수.
     */
    std::string mRealname;
};

}
