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
    NetworkManager(const int port, const std::string& password);
    ~NetworkManager();

    void Run();
private:
    NetworkManager();
    NetworkManager(const NetworkManager& networkManager);
    const NetworkManager& operator=(const NetworkManager& networkManager);

    int createServerSocket();
    int createKqueue();
    int setServerSocket();
    int monitorSocketEvent();

    int addClient(sockaddr_in& clientAddr, int clientSocket);

private:
    const int mPort;
    const std::string& mPassword;
    const int mMaxEventCount; // event 발생시, 발생한 event들을 저장할 수 있는 최대 크기
    int mServerSocket;
    int mKqueue;
    std::map<int, struct session> mSessions;
};
