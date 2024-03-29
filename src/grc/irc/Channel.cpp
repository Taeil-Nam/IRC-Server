#include "Channel.hpp"

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

bool Channel::IsUserExist(const std::string& nickname) const
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

bool Channel::IsOperator(const std::string& nickname) const
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

void Channel::SetName(const std::string& name)
{
    ASSERT(mName == "") << "Old name must empty string(\"\")";
    mName = name;
}

void Channel::AddUser(const std::string& nickname)
{
    if (mUsers.count(nickname) == 0)
    {
        mUsers.insert(nickname);
    }
}

void Channel::AddOperator(const std::string& nickname)
{
    if (IsUserExist(nickname) && mOperators.count(nickname) == 0)
    {
        mOperators.insert(nickname);
    }
}

void Channel::DeleteUser(const std::string& nickname)
{
    if (mUsers.count(nickname) > 0)
    {
        mUsers.erase(nickname);
    }
}

void Channel::DeleteOperator(const std::string& nickname)
{
    if (mOperators.count(nickname) > 0)
    {
        mOperators.erase(nickname);
    }
}


Channel::~Channel()
{

}

}
