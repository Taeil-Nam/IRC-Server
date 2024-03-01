#include "Network.hpp"
#include <netdb.h>

namespace grc
{

Network::Network()
: mServerSocket(ERROR)
{
    mNewClients.reserve(128);
}

Network::~Network()
{
    close(mServerSocket);
    mSessions.clear();
}

bool Network::Init(const int32 port)
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

void Network::Read(const int32 socket)
{
    if (socket == mServerSocket)
    {
        addClient();
    }
    else
    {
        recvFromClient(socket);
    }
}

void Network::Write(const int32 socket)
{
    sendToClient(socket);
}

int32 Network::GetServerSocket() const
{
    return mServerSocket;
}

const char* Network::GetIP(const int32 socket) const
{
    if (mSessions.find(socket) != mSessions.end())
    {
        return inet_ntoa(mSessions.at(socket).addr.sin_addr);
    }
    return "Unknown client(doesn't have session))";
}

const std::vector<int>& Network::FetchNewClients() const
{
    return mNewClients;
}

void Network::ClearNewClients()
{
    mNewClients.clear();
}

void Network::ClearReceiveBuffer(const int32 socket)
{
    std::map<int, struct Session>::iterator pair = mSessions.find(socket);
    if (pair != mSessions.end())
    {
        std::memset(pair->second.recvBuffer, 0, sizeof(pair->second.recvBuffer));
        pair->second.recvSize = 0;
    }
}

void Network::ClearSendBuffer(const int32 socket)
{
    std::map<int, struct Session>::iterator pair = mSessions.find(socket);
    if (pair != mSessions.end())
    {
        std::memset(pair->second.sendBuffer, 0, sizeof(pair->second.sendBuffer));
        pair->second.sendSize = 0;
    }
}

bool Network::createServerSocket()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to create server socket"
            << "(errno:" << errno << " - " << strerror(errno) << ") on socket()";
        return FAILURE;
    }
    return SUCCESS;
}

bool Network::setServerSocket(const int32 port)
{
    int32 reuseOption = 1; // socket 사용 후, 다시 사용하기 까지의 delay 제거(개발자 테스트 편의용)
    int32 keepaliveOption = 1; // 상대방과 연결이 끊어졌는지 60초마다 확인 (TCP 연결 2시간 뒤부터 keepalive 메세지 전송 시작)
    int32 nodelayOption = 1; // 작은 size의 메세지라도, 모으지 않고 바로 보냄 (Nagle 알고리즘 비활성화)

    // server socket option 설정
    if (setsockopt(mServerSocket, SOL_SOCKET, SO_REUSEADDR, &reuseOption, sizeof(reuseOption)) == ERROR
        || setsockopt(mServerSocket, SOL_SOCKET, SO_KEEPALIVE, &keepaliveOption, sizeof(keepaliveOption)) == ERROR
        || setsockopt(mServerSocket, IPPROTO_TCP, TCP_NODELAY, &nodelayOption, sizeof(nodelayOption)) == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to set socket option on server socket"
            << "(errno:" << errno << " - " << strerror(errno) << ") on setsockopt()";
        return FAILURE;
    }
    // server socket non-blocking 설정
    if (fcntl(mServerSocket, F_SETFL, O_NONBLOCK) == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to set non-blocking fd on server socket"
            << "(errno:" << errno << " - " << strerror(errno) << ") on fcntl()";
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
        LOG(LogLevel::Error) << "Failed to bind server socket "
            << "(errno:" << errno << " - " << strerror(errno) << ") on bind()";
        return FAILURE;
    }
    // server socket listen (TCP 연결 준비)
    if (listen(mServerSocket, SOMAXCONN) == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to listen on server socket"
            << "(errno:" << errno << " - " << strerror(errno) << ") on listen()";
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
    int32 clientSocket = accept(mServerSocket, (sockaddr*)&clientAddr, &clientAddrLength);
    if (clientSocket == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to connect client on server socket"
            << "(errno: " << errno << " - " << strerror(errno) << ") on accept()";
        return;
    }
    // client socket non-blocking 설정
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to set non-blocking fd on client socket"
            << "(errno: " << errno << " - " << strerror(errno) << ") on fcntl()";
        close(clientSocket);
        return;
    }
    // client session 추가
    Session client;
    client.addr = clientAddr;
    client.socket = clientSocket;
    mSessions[clientSocket] = client;
    // 새로운 클라이언트 목록에 추가
    mNewClients.push_back(clientSocket);
}

void Network::recvFromClient(const int32 socket)
{
    // client로부터 메세지 수신 시도
    struct Session& session = mSessions[socket];
    int32 recvLen = recv(socket, session.recvBuffer, sizeof(session.recvBuffer), 0);
    // 오류 발생시
    if (recvLen == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to receive message from client(" << GetIP(socket) << ")"
            << "(errno:" << errno << " - " << strerror(errno) << ") on recv()";
        close(socket);
        mSessions.erase(socket);
        return;
    }
    // 상대방과 연결이 끊긴 경우
    else if (recvLen == 0)
    {
        LOG(LogLevel::Notice) << "Client(" << GetIP(socket) << ") disconnected";
        close(socket);
        mSessions.erase(socket);
        return;
    }
    // 메시지 수신 완료
    session.recvSize = recvLen;
    LOG(LogLevel::Notice) << "Received message from client(" << GetIP(socket) << ") "
        << std::strlen(session.recvBuffer) << "bytes : " << session.recvBuffer;
    // WRITE Test code
    std::memcpy(session.sendBuffer, session.recvBuffer, sizeof(session.recvBuffer));
    ClearReceiveBuffer(socket);
    Write(socket);
}

void Network::sendToClient(const int32 socket)
{
    struct Session& session = mSessions[socket];
    int sendLen = send(socket, session.sendBuffer, std::strlen(session.sendBuffer), 0);
    // 오류 발생시
    if (sendLen == -1)
    {
        LOG(LogLevel::Error) << "Failed to send message to client(" << GetIP(socket) << ")"
            << "(errno:" << errno << " - " << strerror(errno) << ") on send()";
        close(socket);
        mSessions.erase(socket);
        return;
    }
    // 메세지 전송 완료
    session.sendSize = sendLen;
    LOG(LogLevel::Notice) << "Sent message to client(" << GetIP(socket) << ") "
        << sendLen << "bytes : " << session.sendBuffer;
    ClearSendBuffer(socket);
}

}
