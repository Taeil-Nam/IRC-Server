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
			std::cerr << "error accepting socket" << std::endl;
			perror("accept()");
			std::cerr << "errno: " << errno << std::endl;
			close(serverSocket);
			return EXIT_FAILURE;
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
				std::cerr << "error receiving data" << std::endl;
				perror("recv()");
				break;
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
				std::cerr << "error sending data" << std::endl;
				perror("send()");
				std::cerr << "errno: " << errno << std::endl;
				break;
			}
			std::cout << "send data " << sendLength << " bytes." << std::endl;
		}
		close(clientSocket);
	}
	close(serverSocket);

	return 0;
}
