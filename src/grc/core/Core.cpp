#include "Core.hpp"
#include "NetworkManager.hpp"
#include "utils/LogManager.hpp"

namespace grc
{

Core::Core(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
{

}

Core::~Core()
{

}

void Core::Run()
{
    if (NetworkManager::GetInstance().InitNetwork(mPort) == FAILURE)
    {
        return;
    }

    LOG(LogLevel::Notice) << "IRC Server 시작 (Port = " << mPort << ")";
    while (true)
    {
        if (NetworkManager::GetInstance().ProcessNetworkEvent() == FAILURE)
        {
            break;
        }
    }
}

}
