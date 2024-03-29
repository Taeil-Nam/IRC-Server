#include "User.hpp"

namespace grc
{

User::User()
: mbIsAuthenticated(false)
, mbIsRegistered(false)
, mNickname("")
, mUsername("")
, mHostname("")
, mServername("")
, mRealname("")
, mLastPingTime(0)
, mLastPongTime(time(NULL))
{

}

const User& User::operator=(const User& user)
{
    mbIsRegistered = user.mbIsRegistered;
    mNickname = user.mNickname;
    mUsername = user.mUsername;
    mHostname = user.mHostname;
    mServername = user.mServername;
    mRealname = user.mRealname;
    mLastPingTime = user.mLastPingTime;
    mLastPongTime = user.mLastPongTime;
    return *this;
}

User::~User()
{

}

bool User::IsAuthenticated() const
{
    return mbIsAuthenticated;
}

bool User::IsRegistered() const
{
    return mbIsRegistered;
}

const std::string& User::GetNickname() const
{
    return mNickname;
}

const std::string& User::GetUsername() const
{
    return mUsername;
}

const std::string& User::GetHostname() const
{
    return mHostname;
}

const std::string& User::GetServername() const
{
    return mServername;
}

const std::string& User::GetRealname() const
{
    return mRealname;
}

time_t User::GetLastPingTime() const
{
    return mLastPingTime;
}

time_t User::GetLastPongTime() const
{
    return mLastPongTime;
}

void User::SetAuthenticated()
{
    mbIsAuthenticated = true;
}

void User::SetRegistered()
{
    mbIsRegistered = true;
}

void User::SetNickname(const std::string& nickname)
{
    mNickname = nickname;
}

void User::SetUsername(const std::string& username)
{
    mUsername = username;
}

void User::SetHostname(const std::string& hostname)
{
    mHostname = hostname;
}

void User::SetServername(const std::string& servername)
{
    mServername = servername;
}

void User::SetRealname(const std::string& realname)
{
    mRealname = realname;
}

void User::SetLastPingTime(const time_t time)
{
    ASSERT(time >= mLastPingTime)
        << "New time must greater or equal than old time";
    mLastPingTime = time;
}

void User::SetLastPongTime(const time_t time)
{
    ASSERT(time >= mLastPongTime)
        << "New time must greater or equal than old time";
    mLastPongTime = time;
}

}
