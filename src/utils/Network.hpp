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

#define BUFFER_SIZE 1024

namespace grc
{

struct session	// 연결된 client의 정보를 저장하는 구조체
{
    sockaddr_in addr;
    int32 socket;
    char recvBuffer[BUFFER_SIZE];
    char sendBuffer[BUFFER_SIZE];
    int64 recvSize;
    int64 sendSize;
};

class Network
{
public:
    Network();
    ~Network();

    bool Init(const int32 port);
    void Read(const int32 socket);
    // int32 Write(const int32 socket);

    const int32 GetServerSocket() const;
    const char* GetIP(const int32 socket) const;
    const std::vector<int>& FetchNewClients() const;
    void ClearNewClients();
    void ClearReceiveBuffer(const int32 socket);
    void ClearSendBuffer(const int32 socket);
private:
    Network(const Network& Network); // = delete
    const Network& operator=(const Network& Network); // = delete

    bool createServerSocket();
    bool setServerSocket(const int32 port);

    void addClient();
    void recvFromClient(int32 clientSocket);
    //int32 writeToClient();

private:
    int32 mServerSocket;
    std::map<int32, struct session> mSessions;
    std::vector<int> mNewClients;
};

}
