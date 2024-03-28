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

#include "common.hpp"

using namespace gdf;

namespace grc
{

class User
{
public:
    User();
    const User& operator=(const User& User);
    virtual ~User();

    bool IsAuthenticated() const;
    bool IsRegistered() const;
    const std::string& GetNickname() const;
    const std::string& GetUsername() const;
    const std::string& GetHostname() const;
    const std::string& GetServername() const;
    const std::string& GetRealname() const;
    const std::string& GetCurrentChannel() const;

    void SetAuthenticated();
    void SetRegistered();
    void SetNickname(const std::string& nickname);
    void SetUsername(const std::string& username);
    void SetHostname(const std::string& hostname);
    void SetServername(const std::string& servername);
    void SetRealname(const std::string& realname);
private:
    User(const User& User); // = delete

private:
    bool mbIsAuthenticated;
    bool mbIsRegistered;
    std::string mNickname;
    std::string mUsername;
    std::string mHostname;
    std::string mServername;
    std::string mRealname;
};

}
