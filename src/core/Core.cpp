#include "Core.hpp"

Core::Core(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
, mNetworkManager(mPort)
{

}

Core::~Core()
{

}

void Core::Run()
{
    mNetworkManager.Run();
}
