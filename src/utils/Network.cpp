#include "Network.hpp"

namespace grc
{

Network::Network()
{
    mServerSocket = ERROR;
    mNewClients.reserve(128);
}

Network::~Network()
{
    close(mServerSocket);
    mSessions.clear();
}

// 네트워크 통신을 위해 필요한 소켓을 만들고 설정합니다.
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

// 소켓에 발생한 READ 이벤트를 처리합니다.
// 서버 소켓의 경우, client 연결 요청을 허가합니다.
// 클라이언트 소켓의 경우, 해당 소켓으로부터 메세지를 읽어옵니다.
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

const int32 Network::GetServerSocket() const
{
    return mServerSocket;
}

const char* Network::GetIP(const int32 socket) const
{
    if (mSessions.find(socket) != mSessions.end())
    {
        return inet_ntoa(mSessions.at(socket).addr.sin_addr);
    }
    return "Unknown client";
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
    std::map<int, struct session>::iterator pair = mSessions.find(socket);
    if (pair != mSessions.end())
    {
        std::memset(pair->second.recvBuffer, 0, sizeof(pair->second.recvBuffer));
        pair->second.recvSize = 0;
    }
}

void Network::ClearSendBuffer(const int32 socket)
{
    std::map<int, struct session>::iterator pair = mSessions.find(socket);
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
        LOG(LogLevel::Error) << "Server socket 생성 오류(errno:" << errno << " - "
            << strerror(errno) << ") on socket()";
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
    // 새로운 클라이언트 목록에 추가
    mNewClients.push_back(clientSocket);
}

void Network::recvFromClient(int32 clientSocket)
{
    // client로부터 메세지 수신 시도
    struct session& currentSession = mSessions[clientSocket];
    int32 recvLen = recv(clientSocket, currentSession.recvBuffer, sizeof(currentSession.recvBuffer), 0);
    // 오류 발생시
    if (recvLen == ERROR)
    {
        LOG(LogLevel::Error) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")로 부터 메세지를 전달받지 못함(errno:"
            << errno << " - " << strerror(errno) << ") on recv()";
        close(clientSocket);
        mSessions.erase(clientSocket);
        return;
    }
    // 상대방과 연결이 끊긴 경우
    else if (recvLen == 0)
    {
        LOG(LogLevel::Notice) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")와 연결이 끊어짐";
        close(clientSocket);
        mSessions.erase(clientSocket);
        return;
    }
    // 메시지 수신 완료
    currentSession.recvSize += recvLen;
    LOG(LogLevel::Notice) << "Client(" << inet_ntoa(currentSession.addr.sin_addr) << ")로 부터 "
        << std::strlen(currentSession.recvBuffer) + 1 << "bytes 만큼의 메세지 수신\n" << currentSession.recvBuffer << "\n";
}

}
