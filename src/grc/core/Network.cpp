#include "Network.hpp"
#include "Event.hpp"
#include "utils/LogManager.hpp"
#include <cstdio>
#include <unistd.h>

namespace grc
{

Network::Network()
{

}

Network::~Network()
{
    close(mServerSocket);
    mSessions.clear();
}

int Network::InitNetwork(const int port)
{
    if (createServerSocket() == FAILURE)
    {
        return FAILURE;
    }
    if (setServerSocket(port) == FAILURE)
    {
        close(mServerSocket);
        return FAILURE;
    }
    return SUCCESS;
}

int Network::ProcessNetworkEvent()
{
    // event 확인(모니터링)
    struct kevent* eventList = Event.GetEventList();
    int eventCount = Event.getEventCount();
    if (eventCount == ERROR)
    {
        LOG(LogLevel::Error) << "Network event list 생성 오류";
        close(mServerSocket);
        mSessions.clear();
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
            // stdin인 경우
            if (currentSocket == STDIN_FILENO)
            {
                continue;
            }
            // server socket인 경우
            else if (currentSocket == mServerSocket)
            {
                addClient();
            }
            // client socket인 경우
            else
            {
                recvFromClient(currentSocket);
            }
        }

        // WRITE 이벤트 처리
        else if (currentEvent.filter == EVFILT_WRITE)
        {
            if (currentSocket == STDOUT_FILENO)
            {
                continue;
            }
            else
            {
                // writeToClient(int clientSocket);
            }
        }
    }
    return SUCCESS;
}

int Network::createServerSocket()
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

int Network::setServerSocket(const int port)
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
    serverAddress.sin_port = htons(port);
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
    if (Event.AddReadEvent(mServerSocket) == FAILURE)
    {
        LOG(LogLevel::Error) << "Server socket READ event 등록 오류 on Event::AddReadEvent()";
        return FAILURE;
    }

    return SUCCESS;
}

void Network::addClient()
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
        return;
    }

    // client socket non-blocking 설정
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == ERROR)
    {
        LOG(LogLevel::Error) << "client socket non-blocking 설정 오류(errno: " << errno << " - "
            << strerror(errno) << ") on fcntl()";
        close(clientSocket);
        return;
    }

    // client session 추가
    session client;
    client.addr = clientAddr;
    client.socket = clientSocket;
    mSessions[clientSocket] = client;

    // client socket에 대한 READ, WRITE event 추가
    if (Event.AddReadEvent(clientSocket) == FAILURE)
    {
        LOG(LogLevel::Error) << "Client socket READ event 등록 오류 on Event::AddReadEvent()";
        return;
    }
    if (Event.AddWriteEvent(clientSocket) == FAILURE)
    {
        LOG(LogLevel::Error) << "Client socket WRITE event 등록 오류 on Event::AddWriteEvent()";
        return;
    }
    LOG(LogLevel::Notice) << "Client(" << inet_ntoa(clientAddr.sin_addr) << ") 연결됨";
}

void Network::recvFromClient(int clientSocket)
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
        return;
    }
    else if (recvLen == 0) // 상대방과 연결이 끊긴 경우
    {
        LOG(LogLevel::Notice) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")와 연결이 끊어짐";
        close(clientSocket);
        mSessions.erase(clientSocket);
        return;
    }
    LOG(LogLevel::Notice) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")로 부터 "
        << std::strlen(currentSession.recvBuffer) + 1 << "bytes 만큼의 메세지 수신\n" << currentSession.recvBuffer << "\n";

    // Todo: 전달 받은 메세지 핸들링
}

}
