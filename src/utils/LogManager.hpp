/**
 * @file LogManager.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief 에러 로거 클래스 헤더
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <vector>
#include <fstream>
#include <sys/_types/_u_int32_t.h>
#include <sys/stat.h>

#if !defined(__clang__) && !defined(__GNUC__)
    #define __PRETTY_FUNCTION__ ("not supported")
#endif

#define LOG(level, msg) LogManager::GetInstance().Log(level, msg, __PRETTY_FUNCTION__)

enum eSeverityLevel // 로그의 심각도 수준
{
    Emergency = 0,  // system is unusable
    Alert,          // action must be taken immediately
    Critical,       // critical conditions
    Error,          // error conditions
    Warning,        // warning conditions
    Notice,         // normal but significant condition
    Informational,  // informational messages
    Debug,          // debug-level messages
};

/**
 * @brief 싱글톤 패턴. 구현된 매크로 함수(LOG) 사용 추천.
 * 따로 delete 필요 없음.
 *
 * @note 파일/폴더 생성 예외처리 안됨. 권한 없을 시, 문제 발생 가능.
 */
class LogManager
{
public:
    LogManager();
    ~LogManager();

    static LogManager& GetInstance();

    void Log(eSeverityLevel, const std::string& message, const char* functionName);
private:
    LogManager(const LogManager&);
    LogManager& operator=(const LogManager&);

    void createLogFile();

    std::string getCurrentTime();

private:
    std::ofstream mLogFile;
};
