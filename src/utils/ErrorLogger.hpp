/**
 * @file ErrorLogger.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief 에러 로거 클래스 헤더
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <fstream>
#include <ctime>
#include <sstream>
#include <sys/stat.h>

#if !defined(__clang__) && !defined(__GNUC__)
    #define __PRETTY_FUNCTION__ ("not supported")
#endif

#define LOG(msg) ErrorLogger::getInstance().log(msg)
#define PRINT(msg) std::cout << (msg) << std::endl

#define FATAL_LOG(msg) ErrorLogger::getInstance().fatalLog(msg, __PRETTY_FUNCTION__)
#define FATAL_PRINT(msg) std::cerr << "<FATAL> " << (msg) << std::endl

/**
 * @brief 싱글톤 패턴. 구현된 매크로 함수 사용 추천.
 * 스택 메모리를 사용함으로 따로 delete 필요 없음.
 *
 * @note 파일/폴더 생성 예외처리 안됨. 권한 없을 시, 문제 발생 가능.
 */
class ErrorLogger
{
public:
    static ErrorLogger& getInstance();
    void log(const std::string& message);
    void fatalLog(const std::string& message, const char* functionName);

private:
    std::ofstream mLogFile;

    ErrorLogger();
    ErrorLogger(const ErrorLogger&);
    ErrorLogger& operator=(const ErrorLogger&);
    ~ErrorLogger(); 

    void createLogFolder();
    std::string nowTimeStr();
};
