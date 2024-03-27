#include "User.hpp"

namespace grc
{

User::User()
: mbIsRegisterd(false)
, mbIsChannelOperator(false)
{

}

const User& User::operator=(const User& user)
{
    mbIsRegisterd = user.mbIsRegisterd;
    mbIsChannelOperator = user.mbIsChannelOperator;
    mNickname = user.mNickname;
    mUsername = user.mUsername;
    mHostname = user.mHostname;
    mServername = user.mServername;
    mRealname = user.mRealname;
    mCurrentChannel = user.mCurrentChannel;
    return *this;
}

bool User::IsRegisterd() const
{
    return mbIsRegisterd;
}

bool User::IsChannelOperator() const
{
    return mbIsChannelOperator;
}

User::~User()
{

}

}
