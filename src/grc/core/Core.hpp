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
#include "../irc/NumericReply.hpp"
#include "../irc/Channel.hpp"
#include "../irc/User.hpp"

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
        READ = EVFILT_READ,
        WRITE = EVFILT_WRITE
    };
    enum eFD
    {
        STDIN = STDIN_FILENO,
        STDOUT = STDOUT_FILENO
    };
    enum eIRCCommand
    {
        kPass = 0,
        kNick,
        kUser,
        kQuit,
        kJoin,
        kPart,
        kMode,
        kTopic,
        kInvite,
        kKick,
        kPrivmsg,
        kPing,
        kPong,
        kIRCCommandSize
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

    /* about IRC */
    void handleIRCMessage(const int32 IN socket);
    void processPASSMessage(const int32 IN socket,
        const std::vector<std::string>& IN leading, const std::string& IN trailing);
    void processNICKMessage(const int32 IN socket,
        const std::vector<std::string>& IN leading, const std::string& IN trailing);
    void processUSERMessage(const int32 IN socket,
        const std::vector<std::string>& IN leading, const std::string& IN trailing);
    void processQUITMessage(const int32 IN socket,
        const std::vector<std::string>& IN leading, const std::string& IN trailing);
    void processJOINMessage(const int32 IN socket, const std::vector<std::string>& IN leading);
    // void processPARTMessage(const int32 IN socket,
        // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // void processMODEMessage(const int32 IN socket,
        // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // void processTOPICMessage(const int32 IN socket,
        // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // void processINVITEMessage(const int32 IN socket,
        // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // void processKICKMessage(const int32 IN socket,
        // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // void processPRIVMSGMessage(const int32 IN socket,
        // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    void processPINGMessage(const int32 IN socket, const std::vector<std::string>& IN leading);
    void processPONGMessage(const int32 IN socket, const std::vector<std::string>& IN leading);

    bool isNicknameInUse(const std::string& IN nickname);
    void checkUserConnection(const int32 IN socket);

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
    std::map<std::string, Channel> mChannels;
    std::map<int32, User> mUsers;
    std::string mIRCCommand[kIRCCommandSize];
};

}
