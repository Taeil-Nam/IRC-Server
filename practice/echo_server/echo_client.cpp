#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 194;
const int BUFFER_SIZE = 1024;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Invalid argument" << std::endl;
	}

    // 소켓 생성
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
	{
        std::cerr << "Error creating socket." << std::endl;
        return EXIT_FAILURE;
    }

    // 서버 주소 설정
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);  // 서버의 IP 주소
    serverAddr.sin_port = htons(PORT);

    // 서버에 연결
    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
	{
        std::cerr << "Error connecting to server." << std::endl;
        close(clientSocket);
        return EXIT_FAILURE;
    }

    std::cout << "Connected to server." << std::endl;

    // 에코 메시지 전송 및 수신
    char message[BUFFER_SIZE];
    while (true)
	{
        std::cout << "Enter message (or 'exit' to quit): ";
        std::cin.getline(message, sizeof(message));

        if (std::strcmp(message, "exit") == 0) 
		{
            break;
        }

        // 메시지 전송
        send(clientSocket, message, std::strlen(message), 0);

        // 에코 메시지 수신
        int bytesRead = recv(clientSocket, message, sizeof(message), 0);
        if (bytesRead <= 0)
		{
            std::cerr << "Error receiving data from server." << std::endl;
            break;
        }

        message[bytesRead] = '\0';
        std::cout << "Received from server: " << message << std::endl;
    }

    // 소켓 닫기
    close(clientSocket);

    return EXIT_SUCCESS;
}
