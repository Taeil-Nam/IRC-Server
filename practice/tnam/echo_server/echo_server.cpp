// echo server
// 1. 소켓 설정
// 2. 소켓 바인딩
// 3. 소켓 listen
// 4. client aceept
// 5. 데이터 수신
// 6. 데이터 전송

#include <iostream>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <fcntl.h>

int main()
{
	// 1. 소켓 설정
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		std::cerr << "error creating socket." << std::endl;
		return EXIT_FAILURE;
	}

	// setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
	// SO_REUSEADDR = 서버가 다시 시작되었을 때, 이전에 binding 했던 주소 바로 사용 가능.
	// SO_KEEPALIVE = TCP 소켓일시, 상대방 연결이 끊어졌는지 주기적으로 확인 가능.
	// TCP_NODELY = TCP 통신시 Nagle 알고리즘 비활성화. (데이터가 쌓일 때까지 기다리지 않고 작은 데이터라도 바로 보냄)
	int reuseOption = 1;
	int keepaliveOption = 1;
	int nodelayOption = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseOption, sizeof(reuseOption)) == -1
		|| setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, &keepaliveOption, sizeof(keepaliveOption)) == -1
		|| setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, &nodelayOption, sizeof(nodelayOption)) == -1)
	{
		std::cerr << "error setting socket option." << std::endl;
		perror("setsockopt()");
		std::cerr << "errno: " << errno << std::endl;
		close(serverSocket);
		return EXIT_FAILURE;
	}
	// non-blocking socket 설정
	// blocking = accept(), recv(), send() 호출시, 작업 완료될 때까지 대기한다. (sleep이기 때문에 CPU 사용 안함)
	// non-blocking = accept(), recv(), send() 호출시, 바로 리턴한다.
		// non-blocking의 이유로 리턴된 경우, EWOULDBLOCK, EAGAIN와 같은 error code로 리턴된다.
		// 해당 error code로 non-blocking으로 인한 리턴인지 확인이 가능하다.
		// 작업이 없어도 바로 리턴하기 때문에, 반복적으로 작업이 있는지 확인해줘야한다. (spinlock처럼 CPU 많이 씀)
		// 다수의 클라이언트가 연결되고 각 클라이언트 소켓들에 작업이 있는지 확인하려면 반복문으로 확인하기는 너무 리소스 소모가 크다.
		// 따라서, 이런 문제 해결을 위해 IO multiplexing 기술을 사용한다.
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "error setting socket to non-blocking" << std::endl;
		perror("fcntl()");
		std::cerr << "errno: " << errno << std::endl;
		close(serverSocket);
		return EXIT_FAILURE;
	}

	// 2. 소켓 바인딩
	sockaddr_in serverAddress;
	const int serverPort = 194;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(serverPort);

	if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1)
	{
		std::cerr << "error binding socket." << std::endl;
		perror("bind()");
		std::cerr << "errno: " << errno << std::endl;
		close(serverSocket);
		return EXIT_FAILURE;
	}
	
	// 3. 소켓 listen
	if (listen(serverSocket, 10) == -1)
	{
		std::cerr << "error listeing socket" << std::endl;
		perror("listen()");
		std::cerr << "errno: " << errno << std::endl;
		close(serverSocket);
		return EXIT_FAILURE;	
	}
	std::cout << "Server is listening on port " << serverPort << std::endl;

	while (true)
	{
		// 4. client accept
		int clientSocket = 0;
		sockaddr_in clientAddr;
		socklen_t clientAddrLength = sizeof(clientAddr);
		clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLength);
		if (clientSocket == -1)
		{
			if (errno == EWOULDBLOCK) // non-blocking으로 인한 return인 경우
			{
				continue;
			}
			else
			{
				std::cerr << "error accepting socket" << std::endl;
				perror("accept()");
				std::cerr << "errno: " << errno << std::endl;
			}
		}
		std::cout << "Client(" << inet_ntoa(clientAddr.sin_addr) << ") connected." << std::endl;
		
		ssize_t sendLength = 0;
		char recvBuffer[1000];
		ssize_t recvLength = 0;
		while (true)
		{
			// Todo: 데이터 수신
			recvLength = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLength < 0)
			{
				if (errno == EAGAIN) // non-blocking으로 인한 return인 경우
				{
					continue;
				}
				else
				{
					std::cerr << "error receiving data" << std::endl;
					perror("recv()");
					break;
				}
			}
			else if (recvLength == 0) // 0 = 상대방 연결 끊긴경우.
			{
				std::cerr << "Client(" << inet_ntoa(clientAddr.sin_addr) << ") disconnected." << std::endl;
				break;
			}
			std::cout << "receive data " << recvLength << " bytes from " << inet_ntoa(clientAddr.sin_addr) 
			<< ": " << recvBuffer << std::endl;

			// Todo: 데이터 전송
			sendLength = send(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (sendLength <= 0)
			{
				if (errno == EAGAIN) // non-blocking으로 인한 return인 경우
				{
					continue;
				}
				else
				{
					std::cerr << "error sending data" << std::endl;
					perror("send()");
					std::cerr << "errno: " << errno << std::endl;
					break;
				}
			}
			std::cout << "send data " << sendLength << " bytes." << std::endl;
		}
		close(clientSocket);
	}
	close(serverSocket);

	return 0;
}
