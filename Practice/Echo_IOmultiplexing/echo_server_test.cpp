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

int main()
{
	// 1. 소켓 설정
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		std::cerr << "error creating socket." << std::endl;
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
		close(serverSocket);
		return EXIT_FAILURE;
	}
	
	// 3. 소켓 listen
	if (listen(serverSocket, 10) == -1)
	{
		std::cerr << "error listeing socket" << std::endl;
		close(serverSocket);
		return EXIT_FAILURE;	
	}
	std::cout << "Server is listening on port " << serverPort << std::endl;

	// 4. client accept
	int clientSocket = 0;
	sockaddr_in clientAddr;
	socklen_t clientAddrLength = sizeof(clientAddr);
	clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLength);
	if (clientSocket == -1)
	{
		std::cerr << "error accepting socket" << std::endl;
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
		if (recvLength <= 0)
		{
			std::cerr << "error receiving data" << std::endl;
			close(serverSocket);
			return EXIT_FAILURE;
		}
		std::cout << "receive data " << recvLength << " bytes. : " << recvBuffer << std::endl;

		// Todo: 데이터 전송
		sendLength = send(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (sendLength <= 0)
		{
			std::cerr << "error sending data" << std::endl;
			close(serverSocket);
			return EXIT_FAILURE;
		}
		std::cout << "send data " << sendLength << " bytes." << std::endl;
	}

	close(clientSocket);
	close(serverSocket);

	return 0;
}
