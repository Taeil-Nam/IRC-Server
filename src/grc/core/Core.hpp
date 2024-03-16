/**
 * @file Core.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief 메인 함수에서 객체 생성되는 코어 클래스
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <fstream>
#include <sys/time.h>
#include <sys/stat.h>

#include "common.hpp"
#include "utils/Event.hpp"
#include "utils/Network.hpp"
#include "utils/DisplayConsole.hpp"

namespace grc
{

class Core
{
public:
    Core(const int port, const std::string& password);
    ~Core();

    bool Init();
    void Run();

private:
    enum eEventType
    {
        READ = EVFILT_READ,
        WRITE = EVFILT_WRITE
    };
    enum eFD
    {
        STDIN = STDIN_FILENO,
        STDOUT = STDOUT_FILENO
    };
private:
    Core();
    Core(const Core& core); // = delete
    const Core& operator=(const Core& core); // = delete

    bool initLog();
    void initConsoleWindow();

    /* about event */
    bool identifyEvent(const int32 fd, const eEventType type, const struct kevent& event);
    bool identifyEvent(const eEventType type, const struct kevent& event);
    void inputToConsole();
    void excuteConsoleCommand();
    void handleLogBuffer();

    /* about connet client */
    void acceptNewClients();

    /* about console print */
    bool isTimePassed(const int64 ms);

private:
    const int mPort;
    const std::string mPassword;
    bool bRunning;
    Event mEvent;
    Network mNetwork;
    int32 mLogFileFD;
    std::string mLogFileName;
    std::string mLogBuffer;
    uint64 mLogBufferIndex;
    DisplayConsole mLogMonitor;
    DisplayConsole mServerMonitor;
    DisplayConsole* mActivatedWindow;
    struct timeval mLastConsoleRefresh;
    
};

}
