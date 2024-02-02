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

static bool checkArgs(const int argc, const char **argv)
{
    // 인자 개수 확인
    if (argc != 3) return false;
    // 포트 유효성 검사
    if (isInt(argv[1]) == false) return false;
    if (std::atoi(argv[1]) >> 16 != 0) return false; // port 범위: 16비트
    // 비밀번호 유효성 검사
    if (std::string(argv[2]).empty()) return false;

    return true;
}

#include "core/ErrorLogger.hpp"

int main(const int argc, const char **argv)
{
    if (checkArgs(argc, argv) == false)
    {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
        ErrorLogger::getInstance().log("Invalid command line Arguments.");
        return EXIT_FAILURE;
    }
    // run
    const int port = std::atoi(argv[1]);
    const std::string password(argv[2]);

    Core server(port, password);

    server.run();
    
    return EXIT_SUCCESS;
}
