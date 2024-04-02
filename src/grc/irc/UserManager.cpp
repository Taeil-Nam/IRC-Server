#include "UserManager.hpp"
#include "BSD-GDF/Assert.hpp"

namespace grc
{

std::map<int32, User> UserManager::sStaticUsers;

User& UserManager::GetUser(const int32 IN socket)
{
    ASSERT(sStaticUsers.count(socket) > 0)
        << "User must exist";
    return sStaticUsers[socket];
}

const std::map<int32, User>& UserManager::GetUsers()
{
    return sStaticUsers;
}

void UserManager::AddUser(const int32 IN socket)
{
    if (sStaticUsers.count(socket) == 0)
    {
        sStaticUsers[socket];
        sStaticUsers[socket].SetSocket(socket);
    }
}

void UserManager::DeleteUser(const int32 socket)
{
    if (sStaticUsers.count(socket) > 0)
    {
        sStaticUsers.erase(socket);
    }
}

}
