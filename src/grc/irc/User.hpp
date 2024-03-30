/**
 * @file User.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief IRC User 클래스
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

class User
{
public:
    User();
    const User& operator=(const User& IN User);
    virtual ~User();

    bool IsAuthenticated() const;
    bool IsRegistered() const;

    int32 GetSocket() const;
    const std::string& GetNickname() const;
    const std::string& GetUsername() const;
    const std::string& GetHostname() const;
    const std::string& GetServername() const;
    const std::string& GetRealname() const;
    time_t GetLastPingSendTime() const;
    time_t GetLastPongRecvTime() const;

    void SetSocket(const int32 IN socket);
    void SetAuthenticated();
    void SetRegistered();
    void SetNickname(const std::string& IN nickname);
    void SetUsername(const std::string& IN username);
    void SetHostname(const std::string& IN hostname);
    void SetServername(const std::string& IN servername);
    void SetRealname(const std::string& IN realname);
    void SetLastPingTime(const time_t IN time);
    void SetLastPongTime(const time_t IN time);
private:
    User(const User& IN User); // = delete

private:
    int32 mSocket;
    bool mbIsAuthenticated;
    bool mbIsRegistered;
    std::string mNickname;
    std::string mUsername;
    std::string mHostname;
    std::string mServername;
    std::string mRealname;
    time_t mLastPingSendTime;
    time_t mLastPongRecvTime;
};

}
