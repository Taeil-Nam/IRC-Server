#include "User.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"

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
, mLastPingSendTime(0)
, mLastPongRecvTime(time(NULL))
{

}

const User& User::operator=(const User& user)
{
    mSocket = user.mSocket;
    mbIsRegistered = user.mbIsRegistered;
    mNickname = user.mNickname;
    mUsername = user.mUsername;
    mHostname = user.mHostname;
    mServername = user.mServername;
    mRealname = user.mRealname;
    mLastPingSendTime = user.mLastPingSendTime;
    mLastPongRecvTime = user.mLastPongRecvTime;
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

int32 User::GetSocket() const
{
    return mSocket;
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

time_t User::GetLastPingSendTime() const
{
    return mLastPingSendTime;
}

time_t User::GetLastPongRecvTime() const
{
    return mLastPongRecvTime;
}

void User::SetSocket(const int32 IN socket)
{
    mSocket = socket;
}

void User::SetAuthenticated()
{
    mbIsAuthenticated = true;
    LOG(LogLevel::Informational) << "User [" << mNickname << "] Authenticated";
}

void User::SetRegistered()
{
    mbIsRegistered = true;
    LOG(LogLevel::Informational) << "User [" << mNickname << "] Registered";
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
    ASSERT(time >= mLastPingSendTime)
        << "New time must greater or equal than old time";
    mLastPingSendTime = time;
}

void User::SetLastPongTime(const time_t time)
{
    ASSERT(time >= mLastPongRecvTime)
        << "New time must greater or equal than old time";
    mLastPongRecvTime = time;
}

}
