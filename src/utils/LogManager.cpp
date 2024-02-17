
#include "LogManager.hpp"
#include <pthread.h>

LogManager::eSeverityLevel LogManager::mConsoleLevel = LogManager::Notice;
LogManager::eSeverityLevel LogManager::mFileLevel = LogManager::Informational;

LogManager::LogManager()
: mLevelStr(8)
{
    gethostname(mHostname, sizeof(mHostname));
    createLogFile();
    mLevelStr[Emergency] = "Emergency";
    mLevelStr[Alert] = "Alert";
    mLevelStr[Critical] = "Critical";
    mLevelStr[Error] = "Error";
    mLevelStr[Warning] = "Warning";
    mLevelStr[Notice] = "Notice";
    mLevelStr[Informational] = "Informational";
    mLevelStr[Debug] = "Debug";
    pthread_mutex_init(&mConsoleMutex, NULL);
    pthread_mutex_init(&mFileMutex, NULL);
}

LogManager::~LogManager()
{
    if (mLogFile.is_open())
        mLogFile.close();
    pthread_mutex_destroy(&mConsoleMutex);
    pthread_mutex_destroy(&mFileMutex);
}

LogManager& LogManager::GetInstance()
{
    static LogManager instance;
    return instance;
}

void LogManager::ConsoleLog(eSeverityLevel level, const std::string& message,
                            const char* functionName, const char* fileName,
                            const int lineNumber)
{
    pthread_mutex_lock(&mConsoleMutex);
    std::string currentTime = getCurrentTime();

    if (level <= Error)
    {
        std::cerr << "[" << mLevelStr[level] << "] "
                  << currentTime << " "
                  << mHostname << " : "
                  << message
                  << std::endl;
    }
    else
    {
        std::cout << "[" << mLevelStr[level] << "] "
                  << currentTime << " "
                  << mHostname << " : "
                  << message;
        if (level == Debug)
        {
            std::cout << " -> "
                      << fileName
                      << ":"
                      << lineNumber
                      << ": "
                      << functionName;
        }
        std::cout << std::endl;
    }
    pthread_mutex_unlock(&mConsoleMutex);
}

void LogManager::FileLog(eSeverityLevel level, const std::string& message,
                         const char* functionName, const char* fileName,
                         const int lineNumber)
{
    pthread_mutex_lock(&mFileMutex);
    std::string currentTime = getCurrentTime();

    if (mLogFile.is_open())
    {
        mLogFile << "[" << mLevelStr[level] << "] "
                 << currentTime << " "
                 << mHostname << " : "
                 << message;
        if (level == Debug)
        {
            mLogFile << " -> "
                     << fileName
                     << ":"
                     << lineNumber
                     << ": "
                     << functionName;
        }
        mLogFile << std::endl;
    }
    pthread_mutex_unlock(&mFileMutex);
}

LogManager::LogStream::LogStream(eSeverityLevel level, const char* functionName,
                                 const char* fileName, const int lineNumber)
: mLevel(level)
, mFunctionName(functionName)
, mFileName(fileName)
, mLineNumber(lineNumber)
{}

LogManager::LogStream::~LogStream()
{
    if (mLevel <= LogManager::mConsoleLevel)
        LogManager::GetInstance().ConsoleLog(mLevel, mStream.str(),
                                             mFunctionName, mFileName,
                                             mLineNumber);
    if (mLevel <= LogManager::mFileLevel)
        LogManager::GetInstance().FileLog(mLevel, mStream.str(),
                                          mFunctionName, mFileName,
                                          mLineNumber);
}

void LogManager::createLogFile()
{
    std::string fileName;
    struct stat st;

    if (stat("log", &st) != 0)
        mkdir("log", 0755);
    fileName = fileName + "log/log_" + getCurrentTime() + ".txt";
    mLogFile.open(fileName.c_str(), std::ios::app);
}

std::string LogManager::getCurrentTime()
{
    std::time_t current = std::time(NULL);
    std::tm* localTime = std::localtime(&current);
    std::ostringstream time;

    time << (localTime->tm_year + 1900) << '-'
             << std::setfill('0') << std::setw(2) << (localTime->tm_mon + 1) << '-'
             << std::setw(2) << localTime->tm_mday << 'T'
             << std::setw(2) << localTime->tm_hour << ':'
             << std::setw(2) << localTime->tm_min << ':'
             << std::setw(2) << localTime->tm_sec;
    return time.str();
}
