#include "NetworkManager.hpp"
#include "utils/LogManager.hpp"

namespace grc
{

NetworkManager::NetworkManager(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
{

}

NetworkManager::~NetworkManager()
{
    mSessions.clear();
}

NetworkManager& NetworkManager::GetInstance()
{
    static NetworkManager instance;
    return instance;
}

int NetworkManager::InitNetwork()
{
    if (createServerSocket() == FAILURE)
    {
        return FAILURE;
    }
    if (createKqueue() == FAILURE)
    {
        close(mServerSocket);
        return FAILURE;
    }
    if (setServerSocket() == FAILURE)
    {
        close(mServerSocket);
        close(mKqueue);
        return FAILURE;
    }
    return SUCCESS;
}

int NetworkManager::createServerSocket()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket == ERROR)
    {
        LOG(LogLevel::Error) << "Server socket 생성 오류(errno:" << errno << " - "
            << strerror(errno) << ") on socket()";
        return FAILURE;
    }
    return SUCCESS;
}

int NetworkManager::createKqueue()
{
    mKqueue = kqueue();
    if (mKqueue == ERROR)
    {
        LOG(LogLevel::Error) << "Kqueue 생성 오류(errno:" << errno << " - "
            << strerror(errno) << ") on kqueue()";
        return FAILURE;
    }
    return SUCCESS;
}

int NetworkManager::setServerSocket()
{
    int reuseOption = 1;
    int keepaliveOption = 1;
    int nodelayOption = 1;

    // server socket option 설정
    if (setsockopt(mServerSocket, SOL_SOCKET, SO_REUSEADDR, &reuseOption, sizeof(reuseOption)) == ERROR
        || setsockopt(mServerSocket, SOL_SOCKET, SO_KEEPALIVE, &keepaliveOption, sizeof(keepaliveOption)) == ERROR
        || setsockopt(mServerSocket, IPPROTO_TCP, TCP_NODELAY, &nodelayOption, sizeof(nodelayOption)) == ERROR)
    {
        LOG(LogLevel::Error) << "Server socket 옵션 설정 오류(errno:" << errno << " - "
            << strerror(errno) << ") on setsockopt()";
        return FAILURE;
    }

    // server socket non-blocking 설정
    if (fcntl(mServerSocket, F_SETFL, O_NONBLOCK) == ERROR)
    {
        LOG(LogLevel::Error) << "Server socket non-blocking 설정 오류(errno:" << errno << " - "
            << strerror(errno) << ") on fcntl()";
        return FAILURE;
    }

    // server socket address 설정 (IP address, port 바인딩)
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(mPort);
    if (bind(mServerSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == ERROR)
    {
        LOG(LogLevel::Error) << "Server socket bind 오류(errno:" << errno << " - "
            << strerror(errno) << ") on bind()";
        return FAILURE;
    }

    // server socket listen (TCP 연결 준비)
    if (listen(mServerSocket, SOMAXCONN) == ERROR)
    {
        LOG(LogLevel::Error) << "Server socket listen 오류(errno:" << errno << " - "
            << strerror(errno) << ") on listen()";
        return FAILURE;
    }

    // server socket의 READ event를 kqueue에 등록
    struct kevent serverEvent;
    EV_SET(&serverEvent, mServerSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mKqueue, &serverEvent, 1, NULL, 0, NULL) == ERROR)
    {
        LOG(LogLevel::Error) << "Server socket event 등록 오류(errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return FAILURE;
    }

    return SUCCESS;
}

int NetworkManager::monitorSocketEvent()
{
    struct kevent* eventList = new struct kevent[MAX_KEVENT_SIZE];

    LOG(LogLevel::Notice) << "IRC Server 시작 (Port = " << mPort << ")";
    // ConsoleManager.GetInstance().out() << "IRC Server 시작 (Port = " << mPort << ")";
    while (true)
    {
        // event 확인(모니터링)
        std::memset(eventList, 0, sizeof(struct kevent) * MAX_KEVENT_SIZE);
        int eventCount = kevent(mKqueue, NULL, 0, eventList, MAX_KEVENT_SIZE, NULL);
        if (eventCount == ERROR)
        {
            LOG(LogLevel::Error) << "Server socket monitor 오류(errno:" << errno << " - "
                << strerror(errno) << ") on kevent()";
            delete[] eventList;
            return FAILURE;
        }

        // 발생한 event 처리
        for (int i = 0; i < eventCount; i++)
        {
            struct kevent& currentEvent = eventList[i];
            int currentSocket = currentEvent.ident;

            // READ 이벤트 처리
            if (currentEvent.filter == EVFILT_READ)
            {
                // server socket인 경우
                if (currentSocket == mServerSocket)
                {
                    // client 연결
                    if (addClient() == FAILURE)
                    {
                        continue;
                    }
                }
                // client socket인 경우
                else
                {
                    // client로부터 메세지 수신
                    if (recvFromClient(currentSocket) == FAILURE)
                    {
                        continue;
                    }
                }
            }

            // WRITE 이벤트 처리
            else if (currentEvent.filter == EVFILT_WRITE)
            {
                // Todo: user가 채널에 메세지를 보낸 경우에만 채널에 있는 모든 client에게 send() 하도록 추가 필요.
                // 그 외에는 continue;
            }
        }
    }

    delete[] eventList;
    return SUCCESS;
}

int NetworkManager::addClient()
{
    // client 연결
    sockaddr_in clientAddr;
    std::memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t clientAddrLength = sizeof(clientAddr);
    int clientSocket = accept(mServerSocket, (sockaddr*)&clientAddr, &clientAddrLength);
    if (clientSocket == ERROR)
    {
        LOG(LogLevel::Error) << "client 연결 실패(errno: " << errno << " - "
            << strerror(errno) << ") on accept()";
        return FAILURE;
    }

    // client socket non-blocking 설정
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == ERROR)
    {
        LOG(LogLevel::Error) << "client socket non-blocking 설정 오류(errno: " << errno << " - "
            << strerror(errno) << ") on fcntl()";
        close(clientSocket);
        return FAILURE;
    }

    // client session 추가
    session client;
    client.addr = clientAddr;
    client.socket = clientSocket;
    mSessions[clientSocket] = client;

    // client socket에 대한 READ, WRITE event 추가
    struct kevent clientEvent[2];
    EV_SET(&clientEvent[0], clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    EV_SET(&clientEvent[1], clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mKqueue, &clientEvent[0], 2, NULL, 0, NULL) == ERROR)
    {
        LOG(LogLevel::Error) << "client socket event 등록 오류(errno: " << errno << " - "
            << strerror(errno) << ") on kevent()";
        close(clientSocket);
        mSessions.erase(clientSocket);
        return FAILURE;
    }
    LOG(LogLevel::Notice) << "Client(" << inet_ntoa(clientAddr.sin_addr) << ") 연결됨";

    return SUCCESS;
}

int NetworkManager::recvFromClient(int clientSocket)
{
    // client로부터 메세지 수신
    struct session& currentSession = mSessions[clientSocket];
    int recvLen = recv(clientSocket, currentSession.recvBuffer, sizeof(currentSession.recvBuffer), 0);
    if (recvLen == ERROR)
    {
        LOG(LogLevel::Error) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")로 부터 메세지를 전달받지 못함(errno:"
            << errno << " - " << strerror(errno) << ") on recv()";
        close(clientSocket);
        mSessions.erase(clientSocket);
        return FAILURE;
    }
    else if (recvLen == 0) // 상대방과 연결이 끊긴 경우
    {
        LOG(LogLevel::Notice) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")와 연결이 끊어짐";
        close(clientSocket);
        mSessions.erase(clientSocket);
        return FAILURE;
    }
    LOG(LogLevel::Notice) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")로 부터 "
        << std::strlen(currentSession.recvBuffer) + 1 << "bytes 만큼의 메세지 수신\n" << currentSession.recvBuffer << "\n";

    // Todo: 전달 받은 메세지 핸들링

    return SUCCESS;
}

}
