/**
 * @file main.cpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief IRC 서버 어플리케이션의 엔트리 포인트
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "common.hpp"
#include "grc/core/Core.hpp"

/**
 * @brief IRC 서버 어플리케이션의 엔트리 포인트(main 함수)
 * 
 * 이 함수는 커맨드 라인의 인자를 사용하여 IRC 서버를 초기화한다.
 * 정확히 두 개의 인자를 요구하는데, 포트 번호와 서버 비밀번호이다.
 * 포트 번호는 16비트 범위 내의 유효한 정수여야 하며ㅡ 비밀번호는 비어있지 않은 문자열이어야 한다.
 *
 * @param argc 커맨드라인 인자의 수
 * @param argv 커맨드라인 인자 문자열의 배열
 * @return int 성공하면 EXIT_SUCCESS를 반환하고 오류 발생 시 EXIT_FAILURE를 반환
 */
int main(const int IN argc, const char** IN argv)
{
    if (argc != 3
        || isInt(argv[1]) == false
        || std::atoi(argv[1]) >> 16 != 0
        || std::string(argv[2]).empty())
    {
        LOG(LogLevel::Error) << "커맨드라인 인자 오류 (Usage: ./ircserv <port> <password>)";
        return EXIT_FAILURE;
    }
    const int port = std::atoi(argv[1]);
    const std::string password(argv[2]);

    {
        grc::Core server(port, password);
        server.Init();
        server.Run();
    }
    return EXIT_SUCCESS;
}
