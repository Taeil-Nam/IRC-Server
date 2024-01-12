#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 194;
const int BUFFER_SIZE = 1024;

int main()
{
    // 소켓 생성
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP 기반의 소켓 생성.
    if (serverSocket == -1)
	{
        std::cerr << "Error creating socket." << std::endl;
        return EXIT_FAILURE;
    }

    // 서버 주소 설정
    sockaddr_in serverAddr; // 소켓 주소 정보를 저장하는 구조체 생성.
    /* sockaddr_in 구조체 */
		//struct sockaddr_in 
		//{
		//	__uint8_t       sin_len;
		//	sa_family_t     sin_family;	// 주소 체계(일반적으로 IPv4)
		//	in_port_t       sin_port;	// 포트 번호(htons()로 바이트 오더 변환 필요)
		//	struct  in_addr sin_addr;	// IP 주소를 나타내는 구조체. (4 바이트)
		//	char            sin_zero[8];// 구조체 크기를 맞추기 위한 패딩 영역.
		//};
        
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 네트워크 인터페이스를 사용하고 싶은 경우.
	// serverAddr.sin_addr.s_addr = inet_addr(ipAddress); // 특정 IP를 사용하는 네트워크 인터페이스를 사용하고 싶은 경우.
    serverAddr.sin_port = htons(PORT);

    // 소켓에 주소 바인딩
    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
	{
        std::cerr << "Error binding socket." << std::endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    // 연결 대기
    if (listen(serverSocket, 5) == -1)
	{
        std::cerr << "Error listening on socket." << std::endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    // 클라이언트 연결 수락
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    if (clientSocket == -1)
	{
        std::cerr << "Error accepting client connection." << std::endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    std::cout << "Client connected." << std::endl;

    // 에코 메시지 전송 및 수신
    char buffer[BUFFER_SIZE];
    while (true)
	{
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
		{
            std::cerr << "Error receiving data from client." << std::endl;
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;

        // 에코 메시지 전송
        send(clientSocket, buffer, bytesRead, 0);
    }

    // 소켓 닫기
    close(clientSocket);
    close(serverSocket);

    return EXIT_SUCCESS;
}
