#include "GlobalLogger.hpp"

GlobalLogger::GlobalLogger()
: bIsStringTarget(false)
, mStringTarget(NULL)
, mFDTarget(STDOUT_FILENO)
, mLevel(Informational)
, mLevelStr(8)
{
    mLevelStr[Emergency] = "Emergency";
    mLevelStr[Alert] = "Alert";
    mLevelStr[Critical] = "Critical";
    mLevelStr[Error] = "Error";
    mLevelStr[Warning] = "Warning";
    mLevelStr[Notice] = "Notice";
    mLevelStr[Informational] = "Informational";
    mLevelStr[Debug] = "Debug";
    gethostname(mHostname, sizeof(mHostname));
    pthread_mutex_init(&mFileMutex, NULL);
}

GlobalLogger::~GlobalLogger()
{
    pthread_mutex_destroy(&mFileMutex);
}

GlobalLogger& GlobalLogger::GetInstance()
{
    static GlobalLogger instance;
    return instance;
}

void GlobalLogger::Log(eSeverityLevel level, const std::string& message,
                       const char* functionName, const char* fileName,
                       const int lineNumber)
{
    pthread_mutex_lock(&mFileMutex);
    std::string currentTime = getCurrentTime();
    std::stringstream ss;
    ss << "[" << mLevelStr[level] << "] "
       << currentTime << " "
       << mHostname << " : "
       << message;
    if (level == Debug)
    {
        ss << " -> "
           << fileName
           << ":"
           << lineNumber
           << ": "
           << functionName;
    }
    ss << std::endl;
    std::string toWriteString = ss.str();
    if (bIsStringTarget)
    {
        *mStringTarget += toWriteString;
    }
    else
    {
        write(mFDTarget, toWriteString.c_str(), toWriteString.size());
    }
    pthread_mutex_unlock(&mFileMutex);
}

void GlobalLogger::SetTarget(std::string& str)
{
    mStringTarget = &str;
    bIsStringTarget = true;
}

void GlobalLogger::SetTarget(int fd)
{
    if (fd == 0)
        write(1, "GlobalLogger: LOG_SET_TARGET gets stdin(0) file descriptor.\n", 61);
    mFDTarget = fd;
    bIsStringTarget = false;
}

void GlobalLogger::SetLevel(eSeverityLevel level)
{
    mLevel = level;
}

GlobalLogger::LogStream::LogStream(eSeverityLevel level, const char* functionName,
                                   const char* fileName, const int lineNumber)
: mLevel(level)
, mFunctionName(functionName)
, mFileName(fileName)
, mLineNumber(lineNumber)
{}

GlobalLogger::LogStream::~LogStream()
{
    if (mLevel <= GlobalLogger::GetInstance().mLevel)
        GlobalLogger::GetInstance().Log(mLevel, mStream.str(),
                                        mFunctionName, mFileName,
                                        mLineNumber);

}

std::string GlobalLogger::getCurrentTime()
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
