#include "Core.hpp"

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
	/* Log test용 코드 */
	// 사용자 입력이 password랑 다르면 LOG 쌓이도록 테스트 코드 넣어둠
	std::string input;

	std::getline(std::cin, input);
	while (input != "exit")
	{
		if (input != mPassword)
		{
			LOG(Informational, "wrong password(" + input + ")");
		}
		else
		{
			LOG(Informational, "correct password(" + input + ")");
			break;
		}
		std::getline(std::cin, input);
	}
	return;
}
