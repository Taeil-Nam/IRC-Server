#include "ErrorLogger.hpp"

ErrorLogger::ErrorLogger()
{
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream fileName;
    fileName << "error_" 
             << (localTime->tm_year + 1900) << '-'
             << (localTime->tm_mon + 1) << '-'
             << localTime->tm_mday << '_'
             << localTime->tm_hour << '-'
             << localTime->tm_min << '-'
             << localTime->tm_sec << ".log";

    mLogFile.open(fileName.str().c_str(), std::ios::app);
}

ErrorLogger::~ErrorLogger()
{
    if (mLogFile.is_open())
        mLogFile.close();
}

ErrorLogger& ErrorLogger::getInstance() {

    static ErrorLogger instance;
    return instance;
}

void ErrorLogger::log(const std::string& errorMessage) {
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream time;
    time << (localTime->tm_year + 1900) << '-'
             << (localTime->tm_mon + 1) << '-'
             << localTime->tm_mday << '_'
             << localTime->tm_hour << '-'
             << localTime->tm_min << '-'
             << localTime->tm_sec << ": ";
    
    if (mLogFile.is_open()) {
        mLogFile << time.str() << errorMessage << std::endl;
    }
}