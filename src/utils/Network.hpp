/**
 * @file Network.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief 네트워크와 관련된 역할을 수행하는 클래스
 * @version 0.1
 * @date 2024-02-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include "common.hpp"

namespace grc
{

class Network
{
public:
    Network();
    ~Network();

    bool Init(const int32 port);
    const int32 ConnectNewClient();
    void RecvFromClient(const int32 socket);
    void SendToClient(const int32 socket);

    int32 GetServerSocket() const;
    const char* GetIPString(const int32 socket) const;
    void ClearReceiveBuffer(const int32 socket);
    void ClearSendBuffer(const int32 socket);
private:
    Network(const Network& Network); // = delete
    const Network& operator=(const Network& Network); // = delete

    bool createServerSocket();
    bool setServerSocket(const int32 port);

private:
    enum eBufferSize
    {
        RecvBufferSize = 1024,
        SendBufferSize = 1024,
    };
    struct Session
    {
        sockaddr_in addr;
        int32 socket;
        char hostName[NI_MAXHOST];
        char recvBuffer[RecvBufferSize];
        char sendBuffer[SendBufferSize];
        int64 recvSize;
        int64 sendSize;
    };
private:
    int32 mServerSocket;
    std::map<int32, struct Session> mSessions;
};

}
