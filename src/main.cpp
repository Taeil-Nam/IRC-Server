/**
 * @file main.cpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief 메인 어플리케이션
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "common.hpp"
#include "core/Core.hpp"
#include "utils/LogManager.hpp"

int main(const int argc, const char **argv)
{
    if (argc != 3
        || isInt(argv[1]) == false
        || std::atoi(argv[1]) >> 16 != 0
        || std::string(argv[2]).empty())
    {
        LOG(Error, "커맨드라인 인자 오류 (Usage: ./ircserv <port> <password>)");
        return EXIT_FAILURE;
    }

    LOG(Informational, "커맨드라인 인자 정상");
    const int port = std::atoi(argv[1]);
    const std::string password(argv[2]);
    Core server(port, password);

    server.Run();

    return EXIT_SUCCESS;
}
