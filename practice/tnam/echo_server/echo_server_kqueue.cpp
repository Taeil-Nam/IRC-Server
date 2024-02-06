/**
 * @file echo_server_kqueue_test.cpp
 * @author tnam (tnam@student.42seoul.kr)
 * @brief 
 * @version 0.1
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 * @todo client로부터 recvBuffer size보다 많은 데이터를 받은 경우 데이터 처리
 */


#include <cstdlib>
#include <iostream>
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

const int bufferSize = 1000; // recv, send에 사용할 버퍼사이즈
const int serverPort = 194; // server port number

struct session	// 연결된 client의 정보를 저장하는 구조체
{
	sockaddr_in addr;
	int socket;
	char recvBuffer[bufferSize];
	int recvBytes;
	int sendBytes;
	struct kevent event;
};

/**
 * @brief 				error시 오류 내용 출력
 * 
 * @param str 			오류 내용
 * @param function 		실패한 함수 (NULL일 경우 출력 안함)
 */
void printError(const char* str, const char* function)
{
	std::cerr << str << std::endl;
	if (function)
	{
		perror(function);
		std::cerr << "errno: " << errno << std::endl;
	}
}

int main(int argc, char** argv)
{
	// create socket
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		printError("error creating socket.", "socket()");
		return EXIT_FAILURE;
	}

	// set socket option
	int reuseOption = 1;
	int keepaliveOption = 1;
	int nodelayOption = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseOption, sizeof(reuseOption)) == -1
		|| setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, &keepaliveOption, sizeof(keepaliveOption)) == -1
		|| setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, &nodelayOption, sizeof(nodelayOption)) == -1)
	{
		printError("error setting socket option.", "setsockopt()");
		close(serverSocket);
		return EXIT_FAILURE;
	}

	// set non-blocking socket
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		printError("error setting socket to non-blocking.", "fcntl()");
		close(serverSocket);
		return EXIT_FAILURE;
	}

	// bind server address
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(serverPort);
	if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
	{
		printError("error binding socket.", "bind()");
		close(serverSocket);
		return EXIT_FAILURE;
	}

	// listen from server socket
	if (listen(serverSocket, 10) == -1)
	{
		printError("error listeing socket.", "listen()");
		close(serverSocket);
		return EXIT_FAILURE;
	}
	std::cout << "Server is listening on port " << serverPort << std::endl;

	// create kqueue
	int kq = kqueue();
	if (kq == -1)
	{
		printError("error making kqueue.", "kqueue()");
		close(serverSocket);
		close(kq);
		return EXIT_FAILURE;
	}

	// add read event of server socket to kqueue
	struct kevent serverEvent;
	EV_SET(&serverEvent, serverSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &serverEvent, 1, NULL, 0, NULL) == -1)
	{
		printError("error add read event of server socket", "kevent()");
		close(serverSocket);
		close(kq);
		return EXIT_FAILURE;
	}

	// event monitoring and add client event
	std::map<int, struct session> sessions;
	const int maxEventCount = 100;
	struct kevent eventList[maxEventCount];
	while (true)
	{
		std::memset(eventList, 0, sizeof(eventList)); // eventList 초기화.
		int eventCount = kevent(kq, NULL, 0, eventList, maxEventCount, NULL);
		if (eventCount == -1)
		{
			printError("error monitoring event", "kevent()");
			close(serverSocket);
			close(kq);
			return EXIT_FAILURE;
		}

		for (int i = 0; i < eventCount; i++)
		{
			struct kevent& currentEvent = eventList[i];
			int currentSocket = currentEvent.ident;
			if (currentEvent.flags == EV_ERROR)
			{
				if (currentSocket == serverSocket)
				{
					printError("error get event of server socket", NULL);
					// server socket 닫고 return 해야 하는지?
					continue;
				}
				else 
				{
					printError("error get event of client socket", NULL);
					// client socket 닫아줘야 하는지?
					continue;
				}
			}
			else if (currentEvent.filter == EVFILT_READ)
			{
				if (currentSocket == serverSocket)
				{
					sockaddr_in clientAddr;
					std::memset(&clientAddr, 0, sizeof(clientAddr));
					socklen_t clientAddrLength = sizeof(clientAddr);
					int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLength);
					if (clientSocket == -1) // 오류가 발생한 경우
					{
						printError("error accepting client.", "accept()");
						continue;
					}
					else
					{
						if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
						{
							printError("error setting client socket to non-blocking.", "fcntl()");
							close(clientSocket);
							continue;
						}
						session client;
						client.addr = clientAddr;
						client.socket = clientSocket;
						client.recvBytes = 0;
						client.sendBytes = 0;
						sessions[clientSocket] = client; // client session 추가

						struct kevent clientEvent;
						EV_SET(&clientEvent, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
						if (kevent(kq, &clientEvent, 1, NULL, 0, NULL) == -1)
						{
							printError("error add event of client socket", "kevent()");
							close(clientSocket);
							sessions.erase(clientSocket);
						}
						std::cout << "Client(" << inet_ntoa(clientAddr.sin_addr) << ") connected.\n" << std::endl;
					}
				}
				else
				{
					int recvLen = recv(currentSocket, sessions[currentSocket].recvBuffer, sizeof(sessions[currentSocket].recvBuffer), 0);
					if (recvLen == -1) // 오류가 발생한 경우.
					{
						printError("error receiving data from client.", "recv()");
						close(currentSocket);
						sessions.erase(currentSocket); // 해당 client 세션 삭제
						continue;
					}
					else if (recvLen == 0) // 상대방과 연결이 끊긴 경우
					{
						std::cout << "Client(" << inet_ntoa(sessions[currentSocket].addr.sin_addr) << ") disconnected.\n" << std::endl;
						close(currentSocket);
						sessions.erase(currentSocket); // 해당 client 세션 삭제
						continue;
					}
					else
					{
						sessions[currentSocket].recvBytes = recvLen;
						std::cout << "receive data " << recvLen << " bytes from " << inet_ntoa(sessions[currentSocket].addr.sin_addr)
						<< ": " << sessions[currentSocket].recvBuffer << std::endl;
						if (sessions[currentSocket].recvBytes > sessions[currentSocket].sendBytes)
						{
							struct kevent clientEvent;
							EV_SET(&clientEvent, currentSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
							if (kevent(kq, &clientEvent, 1, NULL, 0, NULL) == -1)
							{
								printError("error add write event of client socket", "kevent()");
								close(currentSocket);
								sessions.erase(currentSocket);
							}
						}
					}
				}
			}
			else if (currentEvent.filter == EVFILT_WRITE)
			{
				if (sessions[currentSocket].recvBytes != 0)
				{
					int sendLen = send(currentSocket,
						&sessions[currentSocket].recvBuffer[sessions[currentSocket].sendBytes],
						sessions[currentSocket].recvBytes - sessions[currentSocket].sendBytes, 0);
						// send() 호출시, 100 bytes 크기를 보낸다고 호출해도, 커널 sendBuffer가 모자랄 경우 실제로 100 bytes보다 작은 크기(50 bytes)만 보내게 될 수도 있음.
						// 그럴 경우, 다음 send() 호출시 이전에 보냈던 buffer의 왼쪽 부분은 제외하고, 보내지 못했던 부분부터 보내도록 이와 같은 코드를 사용한다.
					if (sendLen == -1) // send 실패시.
					{
						printError("error sending data to client.", "send()");
						sessions.erase(currentSocket); // 해당 세션 삭제.
						continue;
					}
					else
					{
						std::cout << "send data " << sendLen << " bytes to " << inet_ntoa(sessions[currentSocket].addr.sin_addr)
						<< ": " << sessions[currentSocket].recvBuffer << std::endl;
						sessions[currentSocket].sendBytes += sendLen;
						struct kevent clientEvent;
						EV_SET(&clientEvent, currentSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
						if (kevent(kq, &clientEvent, 1, NULL, 0, NULL) == -1)
						{
							printError("error add read event of client socket", "kevent()");
							close(currentSocket);
							sessions.erase(currentSocket);
						}
						if (sessions[currentSocket].recvBytes == sessions[currentSocket].sendBytes)
						{
							sessions[currentSocket].recvBytes = 0;
							sessions[currentSocket].sendBytes = 0;
						}
					}
				}
			}
		}
	}

	close(serverSocket);

	return 0;
}
