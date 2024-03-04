#include "Core.hpp"

namespace grc
{

Core::Core(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
, bRunning(false)
{

}

Core::~Core()
{
    close(mLogFileFDRead);
    close(mLogFileFDWrite);
    LOG_SET_FD(STDOUT_FILENO);
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
            if (identifyEvent(STDIN_FILENO, event))
            {
                inputToConsole();
                excuteConsoleCommand();
            }
            else if (identifyEvent(mLogFileFDRead, event))
            {
                logFileToConsole();
            }
            else if (event.filter == EVFILT_READ)
            {
                mNetwork.Read(event.ident);
                acceptNewClients();
            }
        }
        
        /* IRC 로직 수행 */

        if (isTimePassed(40))
        {
            mActivatedWindow->RefreshConsole();
        }
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
    if (mEvent.AddReadEvent(mLogFileFDRead) == FAILURE)
    {
        LOG(LogLevel::Error) << "Failed to add Log file READ event";
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
    mLogFileFDWrite = open(mLogFileName.c_str(), O_WRONLY | O_CREAT, 0777);
    if (mLogFileFDWrite == -1)
    {
        LOG(LogLevel::Error) << "Failed to open log file";
        return FAILURE;
    }
    mLogFileFDRead = open(mLogFileName.c_str(), O_RDONLY, 0777);
    if (mLogFileFDRead == -1)
    {
        close(mLogFileFDWrite);
        LOG(LogLevel::Error) << "Failed to open log file";
        return FAILURE;
    }
    mLogFileStreamRead.open(mLogFileName);
    if (mLogFileStreamRead.is_open() == false)
    {
        close(mLogFileFDWrite);
        close(mLogFileFDRead);
        return FAILURE;
    }
    LOG_SET_FD(mLogFileFDWrite);
    LOG_SET_LEVEL(LogLevel::Informational);
    return SUCCESS;
}

void Core::initConsoleWindow()
{
    mLogMonitor.SetHeader(std::string(GAMERC_VERSION) + " - Log monitor");
    mLogMonitor.SetHeaderColor(ConsoleWindow::WhiteCharBlueBG);
    mLogMonitor.SetFooterColor(ConsoleWindow::WhiteCharBlueBG);
    mLogMonitor.SetTimestamp(true);
    mServerMonitor.SetHeader(std::string(GAMERC_VERSION) + " - Server monitor");
    mServerMonitor.SetHeaderColor(ConsoleWindow::WhiteCharRedBG);
    mServerMonitor.SetFooterColor(ConsoleWindow::WhiteCharRedBG);
    mServerMonitor.SetTimestamp(false);
    mLogMonitor.Out(std::string("                                                      "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("  ▄████  ▄▄▄       ███▄ ▄███▓▓█████  ██▀███   ▄████▄  "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string(" ██▒ ▀█▒▒████▄    ▓██▒▀█▀ ██▒▓█   ▀ ▓██ ▒ ██▒▒██▀ ▀█  "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("▒██░▄▄▄░▒██  ▀█▄  ▓██    ▓██░▒███   ▓██ ░▄█ ▒▒▓█    ▄ "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("░▓█  ██▓░██▄▄▄▄██ ▒██    ▒██ ▒▓█  ▄ ▒██▀▀█▄  ▒▓▓▄ ▄██▒"), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("░▒▓███▀▒ ▓█   ▓██▒▒██▒   ░██▒░▒████▒░██▓ ▒██▒▒ ▓███▀ ░"), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string(" ░▒   ▒  ▒▒   ▓▒█░░ ▒░   ░  ░░░ ▒░ ░░ ▒▓ ░▒▓░░ ░▒ ▒  ░"), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("  ░   ░   ▒   ▒▒ ░░  ░      ░ ░ ░  ░  ░▒ ░ ▒░  ░  ▒   "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("░ ░   ░   ░   ▒   ░      ░      ░     ░░   ░ ░        "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("      ░       ░  ░       ░      ░  ░   ░     ░ ░      "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("                                             ░        "), grc::ConsoleWindow::Red);
    mLogMonitor.Out(std::string("GameRC v1.0.0                   IRC server application"), grc::ConsoleWindow::Red);
    mActivatedWindow = &mLogMonitor;
    gettimeofday(&mLastConsoleRefresh, NULL);
}

bool Core::identifyEvent(const int32 fd, const struct kevent& event)
{
    if (event.ident == fd && event.filter == EVFILT_READ)
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
    }
    else
    {
        mActivatedWindow->PushBackCommandLine(key);
    }
}

void Core::excuteConsoleCommand()
{
    while (!mActivatedWindow->IsEOF())
    {
        const std::string input = mActivatedWindow->In();
        if (input == "/exit" || input == "/quit")
        {
            bRunning = false;
        }
        else if (mActivatedWindow == &mLogMonitor)
        {
            if (input == "/status")
            {
                mLogMonitor.Out("Server is running",
                                ConsoleWindow::Green);
                mLogMonitor.Out("IP:        127.0.0.1");
                std::stringstream sPort; sPort << mPort;
                mLogMonitor.Out("port:      " + sPort.str());
                std::stringstream sPassword; sPassword << mPassword;
                mLogMonitor.Out("password:  " + sPassword.str());
            }
            else if (input == "/test")
            {
                LOG(LogLevel::Informational) << "Test";
                LOG(LogLevel::Notice) << "Test";
                LOG(LogLevel::Warning) << "Test";
                LOG(LogLevel::Error) << "Test";
                LOG(LogLevel::Critical) << "Test";
                LOG(LogLevel::Alert) << "Test";
                LOG(LogLevel::Emergency) << "Test";
            }
            else
            {
                mLogMonitor.Out("command not found: " +input, ConsoleWindow::Red);
            }
        }
        else if (mActivatedWindow == &mServerMonitor)
        {
            // 빨간 서버 모니터 콘솔창 
        }
    }
}

void Core::logFileToConsole()
{
    char readBuffer;
    std::string line;
    while (read(mLogFileFDRead, &readBuffer, 1))
    {
        if (readBuffer == '\n')
            break;
        line.push_back(readBuffer);
    }
    ASSERT(line[0] == '[');
    const uint64 firstSpace = line.find(' ');
    const uint64 secondSpace = line.find(' ', firstSpace + 1);
    const uint64 thirdSpace = line.find(' ', secondSpace + 1);
    ASSERT(line[firstSpace - 1] == ']');
    line.erase(firstSpace, thirdSpace - firstSpace);
    mLogMonitor.Out(line);
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
