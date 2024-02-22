/**
 * @file NetworkManager.hpp
 * @author tnam (tnam@student.42seoul.kr)
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

namespace grc
{

struct session	// 연결된 client의 정보를 저장하는 구조체
{
    sockaddr_in addr;
    int socket;
    char recvBuffer[IRC_MESSAGE_SIZE];
    char sendBuffer[IRC_MESSAGE_SIZE];
};

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    static NetworkManager& GetInstance();
    int InitNetwork(const int port);
    int ProcessNetworkEvent();
private:
    NetworkManager(const NetworkManager& networkManager);
    const NetworkManager& operator=(const NetworkManager& networkManager);

    int createServerSocket();
    int createKqueue();
    int setServerSocket(const int port);

    void addClient();
    void recvFromClient(int clientSocket);
    //int writeToClient();

private:
    int mServerSocket;
    int mKqueue;
    std::map<int, struct session> mSessions;
    struct kevent mEventList[MAX_KEVENT_SIZE];
};

}
