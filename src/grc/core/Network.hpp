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
#include <sys/types.h>
#include <sys/event.h>
#include <map>
#include "common.hpp"
#include "Event.hpp"

namespace grc
{

struct session	// 연결된 client의 정보를 저장하는 구조체
{
    sockaddr_in addr;
    int socket;
    char recvBuffer[IRC_MESSAGE_SIZE];
    char sendBuffer[IRC_MESSAGE_SIZE];
};

class Network
{
public:
    Network();
    ~Network();

    int InitNetwork(const int port, Event& event);
    int ProcessNetworkEvent(Event& event);
private:
    Network(const Network& Network); // = delete
    const Network& operator=(const Network& Network); // = delete

    int createServerSocket();
    int setServerSocket(const int port, Event& event);

    void addClient(Event& event);
    void recvFromClient(int clientSocket);
    //int writeToClient();

private:
    int mServerSocket;
    std::map<int, struct session> mSessions;
};

}
