#include <ctime>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "LogManager.hpp"

LogManager::LogManager()
{
    gethostname(mHostname, sizeof(mHostname));
    createLogFile();
}

LogManager::~LogManager()
{
    if (mLogFile.is_open())
        mLogFile.close();
}

LogManager& LogManager::GetInstance()
{
    static LogManager instance;
    return instance;
}

void LogManager::Log(eSeverityLevel level, const std::string& message, const char* functionName)
{
    std::string levelStr;
    std::string currentTime = getCurrentTime();

    switch (level)
    {
        case Emergency:
            levelStr = "Emergency";
            break;
        case Alert:
            levelStr = "Alert";
            break;
        case Critical:
            levelStr = "Critical";
            break;
        case Error:
            levelStr = "Error";
            break;
        case Warning:
            levelStr = "Warning";
            break;
        case Notice:
            levelStr = "Notice";
            break;
        case Informational:
            levelStr = "Informational";
            break;
        case Debug:
            levelStr = "Debug";
            break;
    }

    // 파일에 로그 출력
    if (mLogFile.is_open())
    {
        if (level <= Error) // 심각한 로그일 경우
        {
        mLogFile << "[" << levelStr << "] "
                << currentTime << " "
                << mHostname << " : "
                << message 
                << " -> "
                << functionName
                << std::endl;
        }
        else // 일반적인 로그일 경우
        {
        mLogFile << "[" << levelStr << "] "
                << currentTime << " "
                << mHostname << " : "
                << message
                << std::endl;
        }
    }

    // 스트림에 로그 출력
    if (level <= Error) // 심각한 로그일 경우 cerr에 로그 출력
    {
        std::cerr << "[" << levelStr << "] "
                << currentTime << " "
                << mHostname << " : "
                << message 
                << " -> "
                << functionName
                << std::endl;
    }
    else // 일반적인 로그일 경우 cout에 로그 출력
    {
        std::cout << "[" << levelStr << "] "
                << currentTime << " "
                << mHostname << " : "
                << message
                << std::endl;
    }
}

void LogManager::createLogFile()
{
    std::string fileName;
    struct stat st;

    // Log file 폴더 생성
    if (stat("log", &st) != 0)
        mkdir("log", 0644);

    // Log file 생성
    fileName = fileName + "log/log_" + getCurrentTime() + ".txt";
    mLogFile.open(fileName.c_str(), std::ios::app);
}

std::string LogManager::getCurrentTime()
{
    std::time_t current = std::time(NULL);
    std::tm* localTime = std::localtime(&current);
    std::ostringstream time;

    // 월, 일, 시간이 두 자리로 고정된 시간 반환
    time << (localTime->tm_year + 1900) << '-'
             << std::setfill('0') << std::setw(2) << (localTime->tm_mon + 1) << '-'
             << std::setw(2) << localTime->tm_mday << 'T'
             << std::setw(2) << localTime->tm_hour << ':'
             << std::setw(2) << localTime->tm_min << ':'
             << std::setw(2) << localTime->tm_sec;
    return time.str();
}
