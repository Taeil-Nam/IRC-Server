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
    if (listen(serverSocket, 5) == -1) // 연결 대기용 queue size = 5를 사용하여 해당 소켓에서 클라이언트 연결 대기.
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
        // 클라이언트의 정보를 clientAddr에 담고, 해당 서버 소켓에서 연결을 수락한다.
        // 클라이언트와 연결된 소켓을 새로 하나 만들고, 해당 소켓의 파일 디스크립터를 반환한다.
        // 반환된 독립적인 새로운 소켓을 통해 클라이언트와 통신이 가능하다.
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
            // recv() 호출시 입력을 기다린다(block).
            // client와 연결된 소켓에 데이터가 들어오면, buffer에 buffer 크기 만큼 저장한다.
            // 읽은 데이터의 크기를 반환한다.
            // 들어온 데이터가 버퍼 크기보다 큰 경우 생각해줘야 할듯.
        if (bytesRead <= 0)
		{
            std::cerr << "Error receiving data from client." << std::endl;
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;

        // 에코 메시지 전송
        send(clientSocket, buffer, bytesRead, 0);
        // 해당 소켓으로 데이터를 전송할 때까지 기다린다(block).
        // buffer에 있는 데이터 중 bytesRead 만큼의 크기를 전송한다.
        // 전송이 완료되면, 전송한 크기를 반환한다.
    }

    // 소켓 닫기
    close(clientSocket);
    close(serverSocket);

    return EXIT_SUCCESS;
}
