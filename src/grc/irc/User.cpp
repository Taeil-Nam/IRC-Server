#include "User.hpp"

namespace grc
{

User::User()
: mbIsAuthenticated(false)
, mbIsRegistered(false)
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

}
