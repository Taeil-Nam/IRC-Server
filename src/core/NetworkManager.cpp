#include "NetworkManager.hpp"
#include "utils/LogManager.hpp"

NetworkManager::NetworkManager(const int& port)
: mPort(port)
, mMaxEventCount(MAX_KEVENT_SIZE)
{

}

NetworkManager::~NetworkManager()
{
    mSessions.clear();
}

void NetworkManager::Run()
{
    if (createServerSocket() == FAILURE)
    {
        LOG(Error, "Server socket 생성 오류");
        return;
    }
    if (createKqueue() == FAILURE)
    {
        LOG(Error, "Kqueue 생성 오류");
        close(mServerSocket);
        return;
    }
    if (setServerSocket() == FAILURE)
    {
        LOG(Error, "server socket 설정 오류");
        close(mServerSocket);
        close(mKqueue);
        return;
    }
    if (monitorSocketEvent() == FAILURE)
    {
        LOG(Error, "socket 이벤트 모니터링 오류");
        close(mServerSocket);
        close(mKqueue);
        return;
    }
}

int NetworkManager::createServerSocket()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket == ERROR)
    {
        LOG(Error, std::string(strerror(errno)) + "(socket())");
        return FAILURE;
    }
    return SUCCESS;
}

int NetworkManager::createKqueue()
{
    mKqueue = kqueue();
    if (mKqueue == ERROR)
    {
        LOG(Error, std::string(strerror(errno)) + "(kqueue())");
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
        LOG(Error, std::string(strerror(errno)) + "(setsockopt())");
        return FAILURE;
    }

    // server socket non-blocking 설정
    if (fcntl(mServerSocket, F_SETFL, O_NONBLOCK) == ERROR)
    {
        LOG(Error, std::string(strerror(errno)) + "(fcntl())");
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
        LOG(Error, std::string(strerror(errno)) + "(bind())");
        return FAILURE;
    }

    // server socket listen (client의 TCP 연결 대기)
    if (listen(mServerSocket, SOMAXCONN) == ERROR)
    {
        LOG(Error, std::string(strerror(errno)) + "(listen())");
        return FAILURE;
    }

    // server socket의 READ event를 kqueue에 등록
    struct kevent serverEvent;
    EV_SET(&serverEvent, mServerSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mKqueue, &serverEvent, 1, NULL, 0, NULL) == ERROR)
    {
        LOG(Error, std::string(strerror(errno)) + "(kevent())");
        return FAILURE;
    }

    return SUCCESS;
}

int NetworkManager::monitorSocketEvent()
{
    struct kevent* eventList = new struct kevent[MAX_KEVENT_SIZE];

    LOG(Informational, "IRC Server 시작 (Port = " + toString(mPort) + ")");
    while (true)
    {
        // event 발생했는지 확인(모니터링)
        std::memset(eventList, 0, sizeof(struct kevent) * MAX_KEVENT_SIZE);
        int eventCount = kevent(mKqueue, NULL, 0, eventList, MAX_KEVENT_SIZE, NULL);
        if (eventCount == ERROR)
        {
            LOG(Error, std::string(strerror(errno)) + "(kevent())");
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
                // server socket
                if (currentSocket == mServerSocket)
                {
                    // client 추가
                    sockaddr_in clientAddr;
                    std::memset(&clientAddr, 0, sizeof(clientAddr));
                    socklen_t clientAddrLength = sizeof(clientAddr);
                    int clientSocket = accept(mServerSocket, (sockaddr*)&clientAddr, &clientAddrLength);
                    if (clientSocket == ERROR)
                    {
                        LOG(Error, "client 연결 실패 - " + std::string(strerror(errno)) + "(accept())");
                        continue;
                    }
                    else
                    {
                        if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == ERROR)
                        {
                            LOG(Error, "client socket 설정 오류 - " + std::string(strerror(errno)) + "(fcntl())");
                            close(clientSocket);
                            continue;
                        }

                        // client session 추가
                        session client;
                        client.addr = clientAddr;
                        client.socket = clientSocket;
                        client.recvBytes = 0;
                        client.sendBytes = 0;
                        mSessions[clientSocket] = client;

                        // client socket에 대한 READ event 추가
                        struct kevent clientEvent;
                        EV_SET(&clientEvent, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                        if (kevent(mKqueue, &clientEvent, 1, NULL, 0, NULL) == ERROR)
                        {
                            LOG(Error, "client event 추가 오류 - " + std::string(strerror(errno)) + "(kevent())");
                            close(clientSocket);
                            continue;
                        }
                        LOG(Informational, "Client(" + std::string(inet_ntoa(clientAddr.sin_addr)) + ") 연결됨");
                    }
                }
                // client socket
                else
                {
                    struct session& currentSession = mSessions[currentSocket];
                    int recvLen = recv(currentSocket, currentSession.recvBuffer, sizeof(currentSession.recvBuffer), 0);
                    if (recvLen == ERROR)
                    {
                        LOG(Error, "Client(" + std::string(inet_ntoa(currentSession.addr.sin_addr)) + ")로 부터 메세지를 전달받지 못함 - "
                            + std::string(strerror(errno)) + "(recv())");
                        close(currentSocket);
                        mSessions.erase(currentSocket);
                        continue;
                    }
                    else if (recvLen == 0) // 상대방과 연결이 끊긴 경우
                    {
                        LOG(Error, "Client(" + std::string(inet_ntoa(currentSession.addr.sin_addr)) + ")와 연결이 끊어짐");
                        close(currentSocket);
                        mSessions.erase(currentSocket); // 해당 client 세션 삭제
                        continue;
                    }
                    else
                    {
                        LOG(Informational, "Client(" + std::string(inet_ntoa(currentSession.addr.sin_addr)) + ")로부터 "
                            + toString(std::strlen(currentSession.recvBuffer)) + "bytes 만큼의 메세지 수신");
                        std::cout << currentSession.recvBuffer << std::endl;
                        currentSession.recvBytes = recvLen;
                        if (currentSession.recvBytes > currentSession.sendBytes)
                        {
                            struct kevent clientEvent;
                            EV_SET(&clientEvent, currentSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            if (kevent(mKqueue, &clientEvent, 1, NULL, 0, NULL) == ERROR)
                            {
                                LOG(Error, "client event 추가 오류 - " + std::string(strerror(errno)) + "(kevent())");
                                close(currentSocket);
                                mSessions.erase(currentSocket);
                            }
                        }
                    }
                }
            }

            // WRITE 이벤트 처리
            else if (currentEvent.filter == EVFILT_WRITE)
            {

            }
        }
    }

    delete[] eventList;
    return SUCCESS;
}
