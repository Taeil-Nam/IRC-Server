/**
 * @file GlobalLogger.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief 에러 로거 클래스 헤더
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <pthread.h>

#if !defined(__clang__) && !defined(__GNUC__)
    #define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define LOG(level) GlobalLogger::LogStream(level, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define LOG_SET_FD(fd) GlobalLogger::GetInstance().SetFD(fd)
#define LOG_SET_LEVEL(level) GlobalLogger::GetInstance().SetLevel(level)


class GlobalLogger
{
public:
    enum eSeverityLevel
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
    GlobalLogger();
    ~GlobalLogger();
    static GlobalLogger& GetInstance();
    void Log(eSeverityLevel level, const std::string& message,
             const char* functionName, const char* fileName,
             const int lineNumber);
    void SetFD(int fd);
    void SetLevel(eSeverityLevel level);
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
    GlobalLogger(const GlobalLogger&);              // = delete
    GlobalLogger& operator=(const GlobalLogger&);   // = delete
    std::string getCurrentTime(); 
private:
    int mFD;
    eSeverityLevel mLevel;
    std::vector<std::string> mLevelStr;
    pthread_mutex_t mFileMutex;
    char mHostname[256];
};

// for alias
class LogLevel : public GlobalLogger
{};
