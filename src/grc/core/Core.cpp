#include "Core.hpp"

namespace grc
{

Core::Core(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
, bRunning(false)
, mLogBufferIndex(0)
{

}

Core::~Core()
{
    close(mLogFileFD);
    LOG_SET_TARGET(STDOUT_FILENO);
    LOG_SET_LEVEL(LogLevel::Informational);
}

void Core::Run()
{
    while (bRunning)
    {
        const struct kevent* eventList = mEvent.GetEventList();
        const int eventCount = mEvent.GetEventCount();
        for (uint64 i = 0; i < eventCount; ++i)
        {
            const struct kevent& event = eventList[i];
            if (identifyEvent(STDIN, READ, event))
            {
                inputToConsole();
                excuteConsoleCommand();
            }
            else if (identifyEvent(STDOUT, WRITE, event))
            {
                mActivatedWindow->ScreenNonBlockWrite();
            }
            else if (identifyEvent(mLogFileFD, WRITE, event))
            {
                handleLogBuffer();
            }
            else if (identifyEvent(READ, event))
            {
                mNetwork.Read(event.ident);
                acceptNewClients();
            }
            
        }

        /* IRC 로직 수행 */

    }
}

bool Core::Init()
{
    if (initLog() == FAILURE)
    {
        return FAILURE;
    }
    initConsoleWindow();
    if (mEvent.Init() == FAILURE)
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
    if (mEvent.AddReadEvent(STDIN_FILENO) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add STDIN READ event";
        return FAILURE;
    }
    if (mEvent.AddWriteEvent(STDOUT_FILENO) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add STDOUT WRITE event";
        return FAILURE;
    }
    fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);
    if (mEvent.AddWriteEvent(mLogFileFD) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add log file WRITE event";
        return FAILURE;
    }
    if (mEvent.AddReadEvent(mNetwork.GetServerSocket()) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add server socket READ event";
        return FAILURE;
    }
    bRunning = true;
    LOG(LogLevel::Notice) << "IRC Server is ready (Port = " << mPort << ")";
    return SUCCESS;
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
    mLogMonitor.SetHeaderColor(Display::WhiteCharBlueBG);
    mLogMonitor.SetFooterColor(Display::WhiteCharBlueBG);
    mLogMonitor.SetTimestamp(true);
    mServerMonitor.SetHeader(std::string(GAMERC_VERSION) + " - Server monitor");
    mServerMonitor.SetHeaderColor(Display::WhiteCharRedBG);
    mServerMonitor.SetFooterColor(Display::WhiteCharRedBG);
    mServerMonitor.SetTimestamp(false);
    mLogMonitor.PushContent(std::string("                                                      "), grc::Display::Red);
    mLogMonitor.PushContent(std::string("  ▄████  ▄▄▄       ███▄ ▄███▓▓█████  ██▀███   ▄████▄  "), grc::Display::Red);
    mLogMonitor.PushContent(std::string(" ██▒ ▀█▒▒████▄    ▓██▒▀█▀ ██▒▓█   ▀ ▓██ ▒ ██▒▒██▀ ▀█  "), grc::Display::Red);
    mLogMonitor.PushContent(std::string("▒██░▄▄▄░▒██  ▀█▄  ▓██    ▓██░▒███   ▓██ ░▄█ ▒▒▓█    ▄ "), grc::Display::Red);
    mLogMonitor.PushContent(std::string("░▓█  ██▓░██▄▄▄▄██ ▒██    ▒██ ▒▓█  ▄ ▒██▀▀█▄  ▒▓▓▄ ▄██▒"), grc::Display::Red);
    mLogMonitor.PushContent(std::string("░▒▓███▀▒ ▓█   ▓██▒▒██▒   ░██▒░▒████▒░██▓ ▒██▒▒ ▓███▀ ░"), grc::Display::Red);
    mLogMonitor.PushContent(std::string(" ░▒   ▒  ▒▒   ▓▒█░░ ▒░   ░  ░░░ ▒░ ░░ ▒▓ ░▒▓░░ ░▒ ▒  ░"), grc::Display::Red);
    mLogMonitor.PushContent(std::string("  ░   ░   ▒   ▒▒ ░░  ░      ░ ░ ░  ░  ░▒ ░ ▒░  ░  ▒   "), grc::Display::Red);
    mLogMonitor.PushContent(std::string("░ ░   ░   ░   ▒   ░      ░      ░     ░░   ░ ░        "), grc::Display::Red);
    mLogMonitor.PushContent(std::string("      ░       ░  ░       ░      ░  ░   ░     ░ ░      "), grc::Display::Red);
    mLogMonitor.PushContent(std::string("                                             ░        "), grc::Display::Red);
    mLogMonitor.PushContent(std::string("GameRC v1.0.0                   IRC server application"), grc::Display::Red);
    mActivatedWindow = &mLogMonitor;
    gettimeofday(&mLastConsoleRefresh, NULL);
}

bool Core::identifyEvent(const int32 fd, const eEventType type, const struct kevent& event)
{
    if (event.ident == fd && event.filter == type)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Core::identifyEvent(const eEventType type, const struct kevent& event)
{
    if (event.filter == type)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Core::inputToConsole()
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

void Core::excuteConsoleCommand()
{
    std::string prompt;
    while (mActivatedWindow->pollPromptQueue(prompt))
    {
        if (prompt == "/exit" || prompt == "/quit")
        {
            bRunning = false;
        }
        else if (mActivatedWindow == &mLogMonitor)
        {
            if (prompt == "/status")
            {
                mLogMonitor.PushContent("Server is running",
                                        Display::Green);
                mLogMonitor.PushContent("IP:        127.0.0.1");
                std::stringstream sPort; sPort << mPort;
                mLogMonitor.PushContent("port:      " + sPort.str());
                std::stringstream sPassword; sPassword << mPassword;
                mLogMonitor.PushContent("password:  " + sPassword.str());
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
                mLogMonitor.PushContent("command not found: " +prompt, Display::Red);
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
    if (wrote == len)
    {
        mLogBuffer.clear();
        mLogBufferIndex = 0;
    }
}
void Core::acceptNewClients()
{
    const std::vector<int32>& newClients = mNetwork.FetchNewClients();
    for (uint64 i = 0; i < newClients.size(); ++i)
    {
        if (mEvent.AddReadEvent(newClients[i]) == FAILURE)
        {
            continue;
        }
        LOG(LogLevel::Notice) << "Client(" << mNetwork.GetIPString(newClients[i]) << ") connected";
        mNetwork.ClearNewClients();
    }
}

bool Core::isTimePassed(const int64 ms)
{
    struct timeval nowTime;
    gettimeofday(&nowTime, NULL);
    const int64 elapsedTime = (nowTime.tv_sec - mLastConsoleRefresh.tv_sec)
                                * 1000L
                                + (nowTime.tv_usec - mLastConsoleRefresh.tv_usec)
                                / 1000L;
    if (elapsedTime >= ms)
    {
        mLastConsoleRefresh = nowTime;
        return true;
    }
    else
    {
        return false;
    }
}

}
