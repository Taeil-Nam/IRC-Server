/**
 * @file Channel.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief IRC Channel 클래스
 * @version 0.1
 * @date 2024-03-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../../common.hpp"
#include "User.hpp"

namespace grc
{

class Channel
{
#define NONE 0

public:
    Channel();
    const Channel& operator=(const Channel& IN channel);
    virtual ~Channel();

    bool IsUserExist(const std::string& IN nickname) const;
    bool IsOperator(const std::string& IN nickname) const;
    bool IsInviteOnly() const;
    const std::string& GetName() const;
    const std::string& GetTopic() const;
    int32 GetCurrentUserCount() const;
    const std::map<std::string, User>& GetUsers() const;
    const std::map<std::string, User>& GetOperators() const;
    std::string GetAllUsersNickname() const;

    void SetName(const std::string& IN channelName);

    void AddUser(const std::string& IN nickname, const User& user);
    void AddOperator(const std::string& IN nickname, const User& user);
    void DeleteUser(const std::string& IN nickname);
    void DeleteOperator(const std::string& IN nickname);
private:
    Channel(const Channel& IN channel); // = delete

private:
    std::string mName;
    std::string mTopic;
    std::string mKey;
    int32 mMaxUserCount;
    std::map<std::string, User> mUsers;
    std::map<std::string, User> mOperators;
    bool mbIsInviteOnly;
    bool mbCanOperatorSetTopic;
};

}
