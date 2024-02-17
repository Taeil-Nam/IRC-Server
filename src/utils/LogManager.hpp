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

#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#if !defined(__clang__) && !defined(__GNUC__)
    #define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define LOG(level) LogManager::LogStream(level, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define SET_CONSOLE_LEVEL(level) LogManager::mConsoleLevel = level
#define SET_FILE_LEVEL(level) LogManager::mFileLevel = level

class LogManager
{
public:
    LogManager();
    ~LogManager();
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
    
    static LogManager& GetInstance();
    void ConsoleLog(eSeverityLevel level, const std::string& message,
                    const char* functionName, const char* fileName,
                    const int lineNumber);
    void FileLog(eSeverityLevel level, const std::string& message,
                 const char* functionName, const char* fileName,
                 const int lineNumber);
    
    class LogStream
    {
    public:
        LogStream(eSeverityLevel level, const char* functionName,
                  const char* fileName, const int lineNumber);
        ~LogStream();

        template <typename T>
        LogStream& operator<<(const T& message)
        {
            mStream << message;
            return *this;
        }

    private:
        eSeverityLevel mLevel;
        const char* mFunctionName;
        const char* mFileName;
        const int mLineNumber;
        std::ostringstream mStream;
    };

private:
    LogManager(const LogManager&); // = delete
    LogManager& operator=(const LogManager&); // = delete
    void createLogFile();
    std::string getCurrentTime();

public:
    static eSeverityLevel mConsoleLevel;
    static eSeverityLevel mFileLevel;
    
private:
    std::ofstream mLogFile;
    std::vector<std::string> mLevelStr;
    pthread_mutex_t mConsoleMutex;
    pthread_mutex_t mFileMutex;
    char mHostname[256];
};

// for alias
class LogLevel : public LogManager
{};
