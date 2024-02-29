#include "Core.hpp"
#include "common.hpp"

namespace grc
{

Core::Core(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
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
    /* 초기화 */
    if (init() == FAILURE)
    {
        LOG(LogLevel::Error) << "Core 초기화 오류";
        return;
    }

    /* 메인 로직 */
    LOG(LogLevel::Notice) << "IRC Server start (Port = " << mPort << ")";
    struct timeval consoleFrameStrat, consoleFrameEnd;
    gettimeofday(&consoleFrameStrat, NULL);
    while (true)
    {
        /* 이벤트 처리 */
        const struct kevent* eventList = mEvent.GetEventList();
        const int eventCount = mEvent.GetEventCount();
        for (uint64 i = 0; i < eventCount; ++i)
        {
            const struct kevent& event = eventList[i];
            if (event.ident == STDIN_FILENO && event.filter == EVFILT_READ)
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
                while (!mActivatedWindow->IsEOF())
                {
                    const std::string input = mActivatedWindow->In();
                    if (mActivatedWindow == &mLogMonitor && input == "/status")
                    {
                        mActivatedWindow->Out("Server is running", ConsoleWindow::Green);
                    }
                    else if (mActivatedWindow == &mLogMonitor && input == "/test")
                    {
                        LOG(LogLevel::Informational) << "Test";
                        LOG(LogLevel::Notice) << "Test";
                        LOG(LogLevel::Warning) << "Test";
                        LOG(LogLevel::Error) << "Test";
                        LOG(LogLevel::Critical) << "Test";
                        LOG(LogLevel::Alert) << "Test";
                        LOG(LogLevel::Emergency) << "Test";
                    }
                    else if (input == "/exit" || input == "/quit")
                    {
                        return ;
                    }
                    else
                    {
                        mActivatedWindow->Out(input);
                    }
                }
            }
            else if (event.ident == mLogFileFDRead && event.filter == EVFILT_READ)
            {
                char readBuffer;
                std::string line;
                while (read(mLogFileFDRead, &readBuffer, 1))
                {
                    if (readBuffer == '\n')
                        break;
                    line.push_back(readBuffer);
                }
                const uint64 firstSpace = line.find(' ');
                const uint64 secondSpace = line.find(' ', firstSpace + 1);
                const uint64 thirdSpace = line.find(' ', secondSpace + 1);
                line.erase(firstSpace, thirdSpace - firstSpace);
                mActivatedWindow->Out(line);
            }
            else if ((event.ident != STDIN_FILENO && event.ident != mLogFileFDRead) && event.filter == EVFILT_READ)
            {
                mNetwork.Read(event.ident);
            }

        }
        /* 새로운 클라이언트 연결 */
        const std::vector<int32>& newClients = mNetwork.FetchNewClients();
        for (uint64 i = 0; i < newClients.size(); ++i)
        {
            if (mEvent.AddReadEvent(newClients[i]) == FAILURE)
            {
                continue;
            }
            LOG(LogLevel::Notice) << "Client(" << mNetwork.GetIP(newClients[i]) << ") 연결됨";
            mNetwork.ClearNewClients();
        }
        /* IRC 로직 수행 */
        // Todo: IRC 로직 추가

        /* ConsoleWindow 출력 처리 */
        gettimeofday(&consoleFrameEnd, NULL);
        const long elapsedTime = (consoleFrameEnd.tv_sec - consoleFrameStrat.tv_sec)
                                 * 1000L
                                 + (consoleFrameEnd.tv_usec - consoleFrameStrat.tv_usec)
                                 / 1000L;
        if (elapsedTime >= 40)
        {
            mActivatedWindow->RefreshScreen();
            consoleFrameStrat = consoleFrameEnd;
        }
    }
}


bool Core::init()
{
    if (initLog() == FAILURE)
    {
        return FAILURE;
    }
    initConsoleWindow();
    if (mEvent.Init() == FAILURE)
    {
        LOG(LogLevel::Error) << "Event 객체 초기화 오류";
        return FAILURE;
    }

    if (mNetwork.Init(mPort) == FAILURE)
    {
        LOG(LogLevel::Error) << "Network 객체 초기화 오류";
        return FAILURE;
    }

    /* Add basic events */
    if (mEvent.AddReadEvent(STDIN_FILENO) == FAILURE)
    {
        LOG(LogLevel::Error) << "STDIN event 등록 오류";
        return FAILURE;
    }
    if (mEvent.AddReadEvent(mLogFileFDRead) == FAILURE)
    {
        LOG(LogLevel::Error) << "Log file 등록 오류";
        return FAILURE;
    }
    if (mEvent.AddReadEvent(mNetwork.GetServerSocket()) == FAILURE)
    {
        LOG(LogLevel::Error) << "server socket event 등록 오류";
        return FAILURE;
    }

    return SUCCESS;
}

bool Core::initLog()
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
    mLogFileName = "log_" + time.str() + ".txt";
    mLogFileFDWrite = open(mLogFileName.c_str(), O_WRONLY | O_CREAT, 0777);
    if (mLogFileFDWrite == -1)
    {
        return FAILURE;
    }
    mLogFileFDRead = open(mLogFileName.c_str(), O_RDONLY, 0777);
    if (mLogFileFDRead == -1)
    {
        close(mLogFileFDWrite);
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
}

}
