// echo client
// 1. 소켓 생성
// 2. 서버 주소 설정
// 3. 서버와 연결
// 4. 데이터 전송
// 5. 데이터 수신

#include <iostream>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "invalid argument." << std::endl;
		return EXIT_FAILURE;
	}

	// 1. 소켓 생성
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1)
	{
		std::cerr << "error creating socket." << std::endl;
		return EXIT_FAILURE;
	}

	// 2. 서버 주소 설정
	sockaddr_in server;
	char* serverIP = argv[1];
	const int serverPort = 194;
	std::memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(serverPort);

	// 3. 서버와 연결
	if (connect(clientSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server)) == -1)
	{
		std::cerr << "error connecting server." << std::endl;
		close(clientSocket);
		return EXIT_FAILURE;
	}
	std::cout << "Connected server : " << serverIP << std::endl;

	// 4. 데이터 전송
	char sendBuffer[1000];
	ssize_t sendLength = 0;
	char recvBuffer[1000];
	ssize_t recvLength = 0;
	while (true)
	{
		std::cout << "Enter the message(\"exit\" to quit): " << std::endl;
		std::cin.getline(sendBuffer, sizeof(sendBuffer));
		if (std::strcmp(sendBuffer, "exit") == 0)
		{
			break;
		}

		// Todo: 데이터 전송
		sendLength = send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
		if (sendLength <= 0)
		{
			std::cerr << "error sending data." << std::endl;
			close(clientSocket);
			return EXIT_FAILURE;
		}
		std::cout << "send data " << sendLength << " bytes." << std::endl;

		// Todo: 데이터 수신
		recvLength = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLength <= 0)
		{
			std::cerr << "error receiving data." << std::endl;
			close(clientSocket);
			return EXIT_FAILURE;
		}
		std::cout << "receive data " << recvLength << " bytes. : " << recvBuffer << std::endl;
	}
	
	close(clientSocket);

	return 0;
}
