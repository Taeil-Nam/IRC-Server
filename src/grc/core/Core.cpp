#include "Core.hpp"
#include "Network.hpp"
#include "utils/LogManager.hpp"

namespace grc
{

Core::Core(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
, mNetwork(mEvent)
{

}

Core::~Core()
{

}

void Core::Run()
{
    if (mEvent.InitEvent() == FAILURE)
    {
        return;
    }
    if (mNetwork.InitNetwork(mPort) == FAILURE)
    {
        return;
    }

    LOG(LogLevel::Notice) << "IRC Server 시작 (Port = " << mPort << ")";
    while (true)
    {
        if (mNetwork.ProcessNetworkEvent() == FAILURE)
        {
            break;
        }
    }
}

}
