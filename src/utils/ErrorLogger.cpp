#include "ErrorLogger.hpp"

ErrorLogger::ErrorLogger()
{
    createLogFolder();
    std::ostringstream fileName;
    fileName << "log/log_" << currentTimeStr() << ".txt";
    mLogFile.open(fileName.str().c_str(), std::ios::app);
}

ErrorLogger::~ErrorLogger()
{
    if (mLogFile.is_open())
        mLogFile.close();
}

ErrorLogger& ErrorLogger::getInstance()
{
    static ErrorLogger instance;
    return instance;
}

void ErrorLogger::log(const std::string& errorMessage)
{    
    if (mLogFile.is_open())
        mLogFile << currentTimeStr() << ": " << errorMessage << std::endl;
}

void ErrorLogger::fatalLog(const std::string& errorMessage, const char* functionName)
{    
    if (mLogFile.is_open())
    {
        mLogFile << currentTimeStr() << ": "
            << "<FATAL> "
            << errorMessage
            << " -> "
            << functionName
            << std::endl;
    }
}

std::string ErrorLogger::currentTimeStr()
{
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream time;
    time << (localTime->tm_year + 1900) << '-'
             << (localTime->tm_mon + 1) << '-'
             << localTime->tm_mday << '_'
             << localTime->tm_hour << '-'
             << localTime->tm_min << '-'
             << localTime->tm_sec;
    return time.str();
}

void ErrorLogger::createLogFolder()
{
    struct stat st;
    if (stat("log", &st) != 0)
        mkdir("log", 0777);
}
