#include "Core.hpp"
#include "utils/LogManager.hpp"

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
	LOG(Informational, "IRC Server 시작 (Port = " + toString(mPort) + ")");
}
