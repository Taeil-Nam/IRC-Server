#include "UserManager.hpp"
#include "BSD-GDF/Assert.hpp"

namespace grc
{

std::map<int32, User> UserManager::sStaticUsers;

bool UserManager::IsUserExist(int32 IN socket)
{
    if (sStaticUsers.count(socket) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool UserManager::IsUserExist(const std::string& IN nickname)
{
    std::map<int32, User>::const_iterator it = sStaticUsers.begin();
    while (it != sStaticUsers.end())
    {
        if (nickname == it->second.GetNickname())
        {
            return true;
        }
        it++;
    }
    return false;
}

User& UserManager::GetUser(const int32 IN socket)
{
    ASSERT(sStaticUsers.count(socket) > 0)
        << "User must exist";
    return sStaticUsers[socket];
}
 User& UserManager::GetUser(const std::string& IN nickname)
{
    ASSERT(IsUserExist(nickname))
        << "User must exist";
    std::map<int32, User>::iterator it = sStaticUsers.begin();
    while (it != sStaticUsers.end())
    {
        if (nickname == it->second.GetNickname())
        {
            break;
        }
        it++;
    }
    return it->second;
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
