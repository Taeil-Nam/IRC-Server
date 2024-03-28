#include "Core.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"
#include "common.hpp"
#include <sys/event.h>

namespace grc
{

Core::Core(const int IN port, const std::string& IN password)
: mPort(port)
, mPassword(password)
, mbRunning(false)
, mLogBufferIndex(0)
{
    mIRCCommand[kPass] = "PASS";
    mIRCCommand[kNick] = "NICK";
    mIRCCommand[kUser] = "USER";
    mIRCCommand[kQuit] = "QUIT";
    mIRCCommand[kJoin] = "JOIN";
    mIRCCommand[kPart] = "PART";
    mIRCCommand[kMode] = "MODE";
    mIRCCommand[kTopic] = "TOPIC";
    mIRCCommand[kInvite] = "INVITE";
    mIRCCommand[kKick] = "KICK";
    mIRCCommand[kPrivmsg] = "PRIVMSG";
    mIRCCommand[kPing] = "PING";
    mIRCCommand[kPong] = "PONG";
}

Core::~Core()
{
    close(mLogFileFD);
    LOG_SET_TARGET(STDOUT_FILENO);
    LOG_SET_LEVEL(LogLevel::Informational);
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
                    mUsers.erase(event.GetIdentifier());
                    continue;
                }
                handleIRCMessage(event.GetIdentifier());
            }
            else if (event.IsWriteType())
            {
                if (mNetwork.SendToClient(event.GetIdentifier()) == FAILURE)
                {
                    mUsers.erase(event.GetIdentifier());
                    continue;
                }
            }
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
    mLogMonitor.SetHeaderColor(DisplayBuffer::WhiteCharBlueBG);
    mLogMonitor.SetFooterColor(DisplayBuffer::WhiteCharBlueBG);
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
                mLogMonitor.PushContent("Server is running",
                                        DisplayBuffer::Green);
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
    const int32 newClient = mNetwork.ConnectNewClient();
    if (newClient != ERROR)
    {
        if (mKernelQueue.AddReadEvent(newClient) == FAILURE
            || mKernelQueue.AddWriteEvent(newClient) == FAILURE)
        {
            mNetwork.DisconnectClient(newClient);
            return;
        }
        else
        {
            LOG(LogLevel::Notice) << "Client(" << mNetwork.GetIPString(newClient) << ") connected";
        }
    }
    User newUser;
    mUsers[newClient] = newUser;
}

void Core::handleIRCMessage(const int32 IN socket)
{
    std::string message;
    while (mNetwork.PullFromRecvBuffer(socket, message, "\r\n"))
    {
        if (message.size() > 510)
        {
            continue;
        }
        std::vector<std::string> tokens = split(message, ":");
        std::vector<std::string> leading = split(tokens[0], " ");
        std::string trailing;
        if (tokens.size() >= 2)
        {
            trailing = tokens[1];
        }

        User& user = mUsers[socket];
        std::string& command = leading[0];
        if (command == mIRCCommand[kPass])
        {
            processPASSMessage(socket, leading, trailing);
        }
        else if (command == mIRCCommand[kNick] && user.IsAuthenticated())
        {
            processNICKMessage(socket, leading, trailing);
        }
        // else if (command == mIRCCommand[kUser] && user.IsAuthenticated())
        // {
        //     processUSERMessage(socket, leading, trailing);
        // }
        
   }
}

void Core::processPASSMessage(const int32 IN socket,
    const std::vector<std::string>& IN leading, const std::string& IN trailing)
{
    const size_t parameterSize = leading.size() - 1 + (trailing == "" ? 0 : 1);
    const std::string& command = leading[0];
    // ERR_NEEDMOREPARAMS
    if (parameterSize  < 1)
    {
        mNetwork.FetchToSendBuffer(socket,
            std::to_string(ERR_NEEDMOREPARAMS) + " " + command + " :Not enough parameters\r\n");
        mNetwork.SendToClient(socket);
        mNetwork.DisconnectClient(socket);
        return;
    }
    const std::string& password = leading[1];
    User& user = mUsers[socket];
    // ERR_ALREADYREGISTERED
    if (user.IsAuthenticated())
    {
        mNetwork.FetchToSendBuffer(socket, 
            std::to_string(ERR_ALREADYREGISTERED) + " :You may not reregister\r\n");
        return;
    }
    // ERR_PASSWDMISMATCH
    if (mPassword != password)
    {
        mNetwork.FetchToSendBuffer(socket,
            std::to_string(ERR_PASSWDMISMATCH) + " :Password incorrect\r\n");
        mNetwork.SendToClient(socket);
        mNetwork.DisconnectClient(socket);
        return;
    }
    user.SetAuthenticated();
}

void Core::processNICKMessage(const int32 IN socket,
    const std::vector<std::string>& IN leading, const std::string& IN trailing)
{
    const size_t parameterSize = leading.size() - 1 + (trailing == "" ? 0 : 1);
    // ERR_NONICKNAMEGIVEN
    if (parameterSize < 1)
    {
        mNetwork.FetchToSendBuffer(socket,
            std::to_string(ERR_NONICKNAMEGIVEN) + " :No nickname given\r\n");
        return;
    }
    std::string nickname;
    for (size_t i = 1; i < leading.size(); i++)
    {
        nickname += leading[i];
        if (i < leading.size() - 1)
        {
            nickname += " ";
        }
    }
    // ERR_ERRONEUSNICKNAME
    if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '&' || nickname[0] == '#'
        || nickname.find(' ') != std::string::npos || nickname.find(',') != std::string::npos
        || nickname.find('*') != std::string::npos || nickname.find('?') != std::string::npos
        || nickname.find('!') != std::string::npos || nickname.find('@') != std::string::npos
        || nickname.find('.') != std::string::npos || nickname.size() > 9)
    {
        mNetwork.FetchToSendBuffer(socket,
            std::to_string(ERR_ERRONEUSNICKNAME) + " " + nickname + " :Erroneus nickname\r\n");
        return;
    }
    // ERR_NICKNAMEINUSE
    if (isNicknameInUse(nickname))
    {
        mNetwork.FetchToSendBuffer(socket,
            std::to_string(ERR_NICKNAMEINUSE) + " " + nickname + " :Nickname is already in use\r\n");
        return;
    }
    mUsers[socket].SetNickname(nickname);
}

void Core::processUSERMessage(const int32 IN socket,
    const std::vector<std::string>& IN leading, const std::string& IN trailing)
{
    const size_t parameterSize = leading.size() - 1 + (trailing == "" ? 0 : 1);
    const std::string& command = leading[0];
    // ERR_NEEDMOREPARAMS
    if (parameterSize < 4)
    {
        mNetwork.FetchToSendBuffer(socket,
            std::to_string(ERR_NEEDMOREPARAMS) + " " + command + " :Not enough parameters\r\n");
        return;
    }
    User& user = mUsers[socket];
    // ERR_ALREADYREGISTERED
    if (user.IsRegistered())
    {
        mNetwork.FetchToSendBuffer(socket, 
            std::to_string(ERR_ALREADYREGISTERED) + " :You may not reregister\r\n");
        return;
    }
    const std::string& username = leading[1];
    const std::string& hostname = leading[2];
    const std::string& servername = leading[3];
    const std::string& realname = leading[4];
    user.SetUsername(username);
    user.SetHostname(hostname);
    user.SetServername(servername);
    user.SetRealname(realname);
    user.SetRegistered();
    // TODO(tnam) : RPL_WELCOME (001) 
}

// void Core::processQUITMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processJOINMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processPARTMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processMODEMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processTOPICMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processINVITEMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processKICKMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processPRIVMSGMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processPINGMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {

// }

// void Core::processPONGMessage(const int32 IN socket, const std::vector<std::string>& IN tokens)
// {
    
// }

bool Core::isNicknameInUse(const std::string& nickName)
{
    std::map<int, User>::iterator it = mUsers.begin();
    while (it != mUsers.end())
    {
        if (it->second.GetNickname() == nickName)
        {
            return true;
        }
        it++;
    }
    return false;
}

}
