#include "Channel.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"

namespace grc
{

Channel::Channel()
: mName("")
, mTopic("Game Development")
, mKey("")
, mMaxUserCount(NONE)
, mbIsInviteOnly(false)
, mbCanOperatorSetTopic(false)
{

}

const Channel& Channel::operator=(const Channel& IN channel)
{
    mTopic = channel.mTopic;
    mKey = channel.mKey;
    mMaxUserCount = channel.mMaxUserCount;
    mbIsInviteOnly = channel.mbIsInviteOnly;
    mbCanOperatorSetTopic = channel.mbCanOperatorSetTopic;
    return *this;
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

bool Channel::IsInviteOnly() const
{
    return mbIsInviteOnly;
}

const std::string& Channel::GetTopic() const
{
    return mTopic;
}
const std::string& Channel::GetName() const
{
    return mName;
}

int32 Channel::GetCurrentUserCount() const
{
    return mUsers.size();
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
        AllUsersNicknames += it->second.GetNickname() + " ";
        it++;
    }
    AllUsersNicknames.pop_back();
    LOG(LogLevel::Alert) << "All user in channel : " << AllUsersNicknames;
    return AllUsersNicknames;
}

void Channel::SetName(const std::string& IN channelName)
{
    ASSERT(mName == "") << "Old name must empty string(\"\")";
    mName = channelName;
}

void Channel::AddUser(const std::string& IN nickname, const User& IN user)
{
    if (IsUserExist(nickname) == false)
    {
        mUsers[nickname] = user;
    }
}

void Channel::AddOperator(const std::string& IN nickname, const User& user)
{
    if (IsUserExist(nickname) && IsOperator(nickname) == false)
    {
        mOperators[nickname] = user;
    }
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
