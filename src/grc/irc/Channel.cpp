#include "Channel.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"

namespace grc
{

Channel::Channel()
: mName("")
, mTopic("Game Development")
, mMaxUserCount(UNLIMIT)
, mKey("")
, mbIsProtectedTopic(false)
, mbIsInviteOnly(false)
, mbIsLimitedMaxUserCount(false)
, mbIsKeyRequired(false)
{

}

bool Channel::IsUserExist(const std::string& IN nickname) const
{
    if (mUsers.count(nickname) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Channel::IsOperator(const std::string& IN nickname) const
{
    if (mOperators.count(nickname) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Channel::IsProtectedTopic() const
{
    return mbIsProtectedTopic;
}

bool Channel::IsInviteOnly() const
{
    return mbIsInviteOnly;
}

bool Channel::IsKeyRequired() const
{
    return mbIsKeyRequired;
}

bool Channel::IsLimitedMaxUserCount() const
{
    return mbIsLimitedMaxUserCount;
}

bool Channel::IsChannelEmpty() const
{
    if (mUsers.size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

const std::string& Channel::GetTopic() const
{
    return mTopic;
}
const std::string& Channel::GetName() const
{
    return mName;
}

const std::string& Channel::GetKey() const
{
    return mKey;
}

uint32 Channel::GetCurrentUserCount() const
{
    return mUsers.size();
}

uint32 Channel::GetMaxUserCount() const
{
    return mMaxUserCount;
}

const std::map<std::string, User>& Channel::GetUsers() const
{
    return mUsers;
}

const std::map<std::string, User>& Channel::GetOperators() const
{
    return mOperators;
}

std::string Channel::GetAllUsersNickname() const
{
    std::string AllUsersNicknames;
    std::map<std::string, User>::const_iterator it = mUsers.begin();
    while (it != mUsers.end())
    {
        std::string userNickname;
        if (IsOperator(it->first))
        {
            userNickname = "@" + it->first;
        }
        else
        {
            userNickname = it->first;
        }
        AllUsersNicknames += userNickname + " ";
        it++;
    }
    AllUsersNicknames.pop_back();
    return AllUsersNicknames;
}

std::string Channel::GetModeString() const
{
    // 아무 mode도 설정되어 있지 않은 경우, '+'만 추가되어있음.
    std::string modeString("+");
    if (mbIsProtectedTopic)
    {
        modeString.append("t");
    }
    if (mbIsInviteOnly)
    {
        modeString.append("i");
    }
    if (mbIsLimitedMaxUserCount)
    {
        modeString.append("l");
    }
    if (mbIsKeyRequired)
    {
        modeString.append("k");
    }
    return modeString;
}

std::string Channel::GetModeArgument() const
{
    std::string modeArgument("");
    if (mbIsLimitedMaxUserCount)
    {
        modeArgument.append(std::to_string(mMaxUserCount));
        modeArgument.append(" ");
    }
    if (mbIsKeyRequired)
    {
        modeArgument.append(mKey);
    }
    return modeArgument;
}

void Channel::SetName(const std::string& IN channelName)
{
    ASSERT(mName == "") << "Old name must empty string(\"\")";
    mName = channelName;
}

void Channel::SetTopic(const std::string& IN topic)
{
    mTopic = topic;
    LOG(LogLevel::Informational) << "Set topic of Channel " << "[" << mName << "]"
        << " To " << "[" << mTopic << "]" ;
}

void Channel::SetMaxUserCount(uint32 IN maxUserCount)
{
    mMaxUserCount = maxUserCount;
}

void Channel::SetKey(const std::string& IN key)
{
    mKey = key;
}

void Channel::SetProtectedTopic()
{
    mbIsProtectedTopic = true;
}

void Channel::SetInviteOnly()
{
    mbIsInviteOnly = true;
}

void Channel::SetLimitedMaxUserCount()
{
    mbIsLimitedMaxUserCount = true;
}

void Channel::SetKeyRequired()
{
    mbIsKeyRequired = true;
}

void Channel::UnsetProtectedTopic()
{
    mbIsProtectedTopic = false;
}

void Channel::UnsetInviteOnly()
{
    mbIsInviteOnly = false;
}

void Channel::UnsetLimitedMaxUserCount()
{
    mbIsLimitedMaxUserCount = false;
}

void Channel::UnsetKeyRequired()
{
    mbIsKeyRequired = false;
}

void Channel::AddUser(const std::string& IN nickname, const User& IN user)
{
    if (IsUserExist(nickname) == false)
    {
        mUsers[nickname] = user;
    }
}

void Channel::AddOperator(const std::string& IN nickname, const User& IN user)
{
    if (IsUserExist(nickname) && IsOperator(nickname) == false)
    {
        mOperators[nickname] = user;
    }
}
void Channel::DeleteTopic()
{
    mTopic.clear();
    LOG(LogLevel::Informational) << "Detele topic of Channel " << "[" << mName << "]";
}

void Channel::DeleteUser(const std::string& IN nickname)
{
    if (IsUserExist(nickname))
    {
        mUsers.erase(nickname);
    }
}

void Channel::DeleteOperator(const std::string& IN nickname)
{
    if (IsOperator(nickname))
    {
        mOperators.erase(nickname);
    }
}


Channel::~Channel()
{

}

}
