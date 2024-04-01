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

#include "BSD-GDF/Event/KernelEvent.hpp"
#include "common.hpp"
#include "../irc/IRC.hpp"
#include "../irc/UserManager.hpp"

using namespace gdf;

namespace grc
{

class Core
{
public:
    Core(const int IN port, const std::string& IN password);
    ~Core();

    bool Init();
    void Run();

private:
    enum eEventType
    {
        kRead = EVFILT_READ,
        kWrite = EVFILT_WRITE
    };
    enum eFD
    {
        STDIN = STDIN_FILENO,
        STDOUT = STDOUT_FILENO
    };
    
private:
    Core(); // = delete
    Core(const Core& core); // = delete
    const Core& operator=(const Core& core); // = delete

    bool initLog();
    void initConsoleWindow();

    /* about event */
    void handleMonitorInput();
    void handleMonitorCommand();
    void handleLogBuffer();

    /* about network connection */
    void setupNewClient();

private:
    const int32 mPort;
    const std::string mPassword;
    bool mbRunning;
    KernelQueue mKernelQueue;
    Network mNetwork;
    int32 mLogFileFD;
    std::string mLogFileName;
    std::string mLogBuffer;
    uint64 mLogBufferIndex;
    DisplayConsole mLogMonitor;
    DisplayConsole mServerMonitor;
    DisplayConsole* mActivatedWindow;
};

}
