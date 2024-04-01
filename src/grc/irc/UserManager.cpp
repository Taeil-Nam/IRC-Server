#include "UserManager.hpp"
#include "BSD-GDF/Assert.hpp"

namespace grc
{

std::map<int32, User> UserManager::mUsers;

void UserManager::AddUser(const int32 IN socket)
{
    if (mUsers.count(socket) == 0)
    {
        mUsers[socket];
        mUsers[socket].SetSocket(socket);
    }
}

void UserManager::DeleteUser(const int32 socket)
{
    if (mUsers.count(socket) > 0)
    {
        mUsers.erase(socket);
    }
}

User& UserManager::GetUser(const int32 IN socket)
{
    ASSERT(mUsers.count(socket) > 0)
        << "User must exist";
    return mUsers[socket];
}

const std::map<int32, User>& UserManager::GetUsers()
{
    return mUsers;
}

}
