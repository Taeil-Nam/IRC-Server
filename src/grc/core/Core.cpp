#include "Core.hpp"
#include "BSD-GDF/Assert.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"
#include "common.hpp"
#include <string>
#include <sys/event.h>

namespace grc
{


Core::Core(const int IN port, const std::string& IN password)
: mPort(port)
, mPassword(password)
, mbRunning(false)
, mLogBufferIndex(0)
{

}

Core::~Core()
{
    close(mLogFileFD);
    LOG_SET_TARGET(STDOUT_FILENO);
    LOG_SET_LEVEL(LogLevel::Informational);
}

bool Core::Init()
{
    if (initLog() == FAILURE)
    {
        return FAILURE;
    }
    initConsoleWindow();
    if (mKernelQueue.Init() == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to init Event";
        return FAILURE;
    }
    if (mNetwork.Init(mPort) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to init Network";
        return FAILURE;
    }
    /* Add basic events */
    if (mKernelQueue.AddReadEvent(STDIN_FILENO) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add STDIN READ event";
        return FAILURE;
    }
    if (mKernelQueue.AddWriteEvent(STDOUT_FILENO) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add STDOUT WRITE event";
        return FAILURE;
    }
    fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);
    if (mKernelQueue.AddWriteEvent(mLogFileFD) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add log file WRITE event";
        return FAILURE;
    }
    if (mKernelQueue.AddReadEvent(mNetwork.GetServerSocket()) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add server socket READ event";
        return FAILURE;
    }
    mbRunning = true;
    LOG(LogLevel::Notice) << "IRC Server is ready (Port = " << mPort << ")";
    return SUCCESS;
}

void Core::Run()
{
    while (mbRunning)
    {
        KernelEvent event;
        while (mKernelQueue.Poll(event))
        {
            if (event.IdentifyFD(STDIN) && event.IsReadType())
            {
                handleMonitorInput();
                handleMonitorCommand();
            }
            else if (event.IdentifyFD(STDOUT) && event.IsWriteType())
            {
                mActivatedWindow->Refresh();
            }
            else if (event.IdentifyFD(mLogFileFD) && event.IsWriteType())
            {
                handleLogBuffer();
            }
            else if (event.IdentifySocket(mNetwork.GetServerSocket()) && event.IsReadType())
            {
                setupNewClient();
            }
            else if (event.IsReadType())
            {
                if (mNetwork.RecvFromClient(event.GetIdentifier()) == FAILURE)
                {
                    ChannelManager::DeleteUserFromAllChannels(UserManager::GetUser(event.GetIdentifier()));
                    UserManager::DeleteUser(event.GetIdentifier());
                    continue;
                }
                IRC::HandleMessage(event.GetIdentifier(), mNetwork, mPassword);
            }
            else if (event.IsWriteType())
            {
                if (mNetwork.SendToClient(event.GetIdentifier()) == FAILURE)
                {
                    ChannelManager::DeleteUserFromAllChannels(UserManager::GetUser(event.GetIdentifier()));
                    UserManager::DeleteUser(event.GetIdentifier());
                    continue;
                }
            }
        }
    }
}

bool Core::initLog()
{
    struct stat st;
    if (stat("log", &st) != 0)
        mkdir("log", 0755);
    std::time_t current = std::time(NULL);
    std::tm* localTime = std::localtime(&current);
    std::ostringstream time;
    time << (localTime->tm_year + 1900) << '-'
         << std::setfill('0') << std::setw(2) << (localTime->tm_mon + 1) << '-'
         << std::setw(2) << localTime->tm_mday << 'T'
         << std::setw(2) << localTime->tm_hour << ':'
         << std::setw(2) << localTime->tm_min << ':'
         << std::setw(2) << localTime->tm_sec;
    mLogFileName = "log/" + time.str() + ".txt";
    mLogFileFD = open(mLogFileName.c_str(), O_WRONLY | O_CREAT, 0777);
    if (mLogFileFD == -1)
    {
        LOG(LogLevel::Error) << "Failed to open log file";
        return FAILURE;
    }
    fcntl(mLogFileFD, F_SETFL, O_NONBLOCK);
    LOG_SET_TARGET(mLogBuffer);
    LOG_SET_LEVEL(LogLevel::Informational);
    return SUCCESS;
}

void Core::initConsoleWindow()
{
    mLogMonitor.SetHeader(std::string(GAMERC_VERSION) + " - Log monitor");
    mLogMonitor.SetHeaderColor(DisplayBuffer::WriteCharGrayBG);
    mLogMonitor.SetFooterColor(DisplayBuffer::WriteCharGrayBG);
    mLogMonitor.SetTimestamp(true);
    mServerMonitor.SetHeader(std::string(GAMERC_VERSION) + " - Server monitor");
    mServerMonitor.SetHeaderColor(DisplayBuffer::WhiteCharRedBG);
    mServerMonitor.SetFooterColor(DisplayBuffer::WhiteCharRedBG);
    mServerMonitor.SetTimestamp(false);
    mLogMonitor.PushContent(std::string("                                                      "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("  ▄████  ▄▄▄       ███▄ ▄███▓▓█████  ██▀███   ▄████▄  "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string(" ██▒ ▀█▒▒████▄    ▓██▒▀█▀ ██▒▓█   ▀ ▓██ ▒ ██▒▒██▀ ▀█  "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("▒██░▄▄▄░▒██  ▀█▄  ▓██    ▓██░▒███   ▓██ ░▄█ ▒▒▓█    ▄ "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("░▓█  ██▓░██▄▄▄▄██ ▒██    ▒██ ▒▓█  ▄ ▒██▀▀█▄  ▒▓▓▄ ▄██▒"), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("░▒▓███▀▒ ▓█   ▓██▒▒██▒   ░██▒░▒████▒░██▓ ▒██▒▒ ▓███▀ ░"), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string(" ░▒   ▒  ▒▒   ▓▒█░░ ▒░   ░  ░░░ ▒░ ░░ ▒▓ ░▒▓░░ ░▒ ▒  ░"), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("  ░   ░   ▒   ▒▒ ░░  ░      ░ ░ ░  ░  ░▒ ░ ▒░  ░  ▒   "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("░ ░   ░   ░   ▒   ░      ░      ░     ░░   ░ ░        "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("      ░       ░  ░       ░      ░  ░   ░     ░ ░      "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("                                             ░        "), DisplayBuffer::Red);
    mLogMonitor.PushContent(std::string("GameRC v1.0.0                   IRC server application"), DisplayBuffer::Red);
    mActivatedWindow = &mLogMonitor;
}

void Core::handleMonitorInput()
{
    const char key = getchar();
    if (key == '\t')
    {
        if (mActivatedWindow == &mLogMonitor)
            mActivatedWindow = &mServerMonitor;
        else if (mActivatedWindow == &mServerMonitor)
            mActivatedWindow = &mLogMonitor;
        mActivatedWindow->SetIsScreenUpdated(true);
    }
    else
    {
        mActivatedWindow->PushCharPrompt(key);
    }
}

void Core::handleMonitorCommand()
{
    std::string prompt;
    while (mActivatedWindow->PollPromptQueue(prompt))
    {
        if (prompt == "/exit" || prompt == "/quit")
        {
            mbRunning = false;
        }
        else if (mActivatedWindow == &mLogMonitor)
        {
            if (prompt == "/status")
            {
                std::string content;
                
                content = "Server is running";
                mLogMonitor.PushContent(content,DisplayBuffer::Green);
                
                content = "IP:              " + mNetwork.GetIPString(mNetwork.GetServerSocket());
                mLogMonitor.PushContent(content);

                content = "Port:            " + std::to_string(mPort);
                mLogMonitor.PushContent(content);

                content = "Password:        " + mPassword;
                mLogMonitor.PushContent(content);

                content = "Total Users:     " + std::to_string(UserManager::GetUsers().size());
                mLogMonitor.PushContent(content);

                content = "Total Channels:  " + std::to_string(ChannelManager::GetChannels().size());
                mLogMonitor.PushContent(content);
            }
            else if (prompt == "/test")
            {
                LOG(LogLevel::Informational) << "Test";
                LOG(LogLevel::Notice) << "Test";
                LOG(LogLevel::Warning) << "Test";
                LOG(LogLevel::Error) << "Test";
                LOG(LogLevel::Critical) << "Test";
                LOG(LogLevel::Alert) << "Test";
                LOG(LogLevel::Emergency) << "Test";
            }
            else if (prompt == "/clear")
            {
                mLogMonitor.ClearContent();
            }
            else
            {
                mLogMonitor.PushContent("command not found: " +prompt, DisplayBuffer::Red);
            }
        }
        else if (mActivatedWindow == &mServerMonitor)
        {
            // 빨간 서버 모니터 콘솔창 
        }
    }
}

void Core::handleLogBuffer()
{
    if (mLogBuffer.empty())
    {
        return ;
    }

    /* to log file */
    const char *buf = mLogBuffer.c_str();
    const uint64 len = std::strlen(&buf[mLogBufferIndex]);
    const int64 wrote = write(mLogFileFD, &buf[mLogBufferIndex], len);
    if (wrote == -1)
    {
        LOG(LogLevel::Notice) << "Failed to write on logfile.";
        return ;
    }

    /* to log monitor */
    std::string toLogMonitor = mLogBuffer.substr(mLogBufferIndex, wrote);
    std::stringstream ss(toLogMonitor);
    std::string line;
    while (std::getline(ss, line))
    {
        if (line[0] == '[')
        {
            const uint64 firstSpace = line.find(' ');
            const uint64 secondSpace = line.find(' ', firstSpace + 1);
            const uint64 thirdSpace = line.find(' ', secondSpace + 1);
            if (line[firstSpace - 1] == ']')
            {
                ASSERT(firstSpace < thirdSpace) << "Log format has problem";
                line.erase(firstSpace, thirdSpace - firstSpace);
            }
        }
        mLogMonitor.PushContent(line);
    }

    mLogBufferIndex += wrote;
    if ((uint64)wrote == len)
    {
        mLogBuffer.clear();
        mLogBufferIndex = 0;
    }
}

void Core::setupNewClient()
{
    const int32 newClientSocket = mNetwork.ConnectNewClient();
    if (newClientSocket != ERROR)
    {
        if (mKernelQueue.AddReadEvent(newClientSocket) == FAILURE
            || mKernelQueue.AddWriteEvent(newClientSocket) == FAILURE)
        {
            mNetwork.DisconnectClient(newClientSocket);
            return;
        }
        else
        {
            LOG(LogLevel::Notice) << "Client(" << mNetwork.GetIPString(newClientSocket) << ") connected";
        }
    }
    UserManager::AddUser(newClientSocket);
}



}
