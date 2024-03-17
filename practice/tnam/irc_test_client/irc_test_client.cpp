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
#include <netinet/tcp.h>
#include <fcntl.h>

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

	// setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
	// SO_REUSEADDR = 서버가 다시 시작되었을 때, 이전에 binding 했던 주소 바로 사용 가능.
	// SO_KEEPALIVE = TCP 소켓일시, 상대방 연결이 끊어졌는지 주기적으로 확인 가능.
	// TCP_NODELY = TCP 통신시 Nagle 알고리즘 비활성화. (데이터가 쌓일 때까지 기다리지 않고 작은 데이터라도 바로 보냄)
	int reuseOption = 1;
	int keepaliveOption = 1;
	int nodelayOption = 1;
	if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &reuseOption, sizeof(reuseOption)) == -1
		|| setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &keepaliveOption, sizeof(keepaliveOption)) == -1
		|| setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &nodelayOption, sizeof(nodelayOption)) == -1)
	{
		std::cerr << "error setting socket option." << std::endl;
		perror("setsockopt()");
		std::cerr << "errno: " << errno << std::endl;
		close(clientSocket);
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
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "error setting socket to non-blocking" << std::endl;
		perror("fcntl()");
		std::cerr << "errno: " << errno << std::endl;
		close(clientSocket);
		return EXIT_FAILURE;
	}


	// 2. 서버 주소 설정
	sockaddr_in server;
	char* serverIP = argv[1];
	const int serverPort = 6667;
	std::memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(serverPort);

	// 3. 서버와 연결
	while (true)
	{
		if (connect(clientSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server)) == -1)
		{
			if (errno == EINPROGRESS || errno == EALREADY) // non-blocking으로 인한 return인 경우
			{
				continue;
			}
			else if (errno == EISCONN) // 이미 서버에 연결된 경우
			{
				break;
			}
			else
			{
				std::cerr << "error connecting server." << std::endl;
				perror("connect()");
				std::cerr << "errno: " << errno << std::endl;
				return EXIT_FAILURE;
			}
		}
	}
	std::cout << "Connected server : " << serverIP << std::endl;
	std::cout << std::endl;

	// 4. 데이터 전송
	char sendBuffer[512];
	std::memset(sendBuffer, 0, sizeof(sendBuffer));
	ssize_t sendLength = 0;
	char recvBuffer[512];
	ssize_t recvLength = 0;
	while (true)
	{
		std::cout << "Enter the message(\"exit\" to quit): ";
		std::cin.getline(sendBuffer, sizeof(sendBuffer));
		if (std::cin.eof())  // ctrl+d(EOF) 입력시.
		{
			std::clearerr(stdin);
			std::cin.clear();
			std::cout << std::endl;
			continue;
		}
		else if (std::cin.fail()) // buffer 크기보다 더 많은 문자열이 들어온 경우 cin(istream)의 failbit가 설정된다.
		{
			std::cout << "Max input length = 3000!! can't send" << std::endl;
			std::fflush(stdin);	// cin 버퍼 비우기 (안비우면 버퍼에 남아있는게 전송되어버림)
			std::cin.clear();	// cin의 failbit 재설정
			continue;
		}
		else if (std::strcmp(sendBuffer, "exit") == 0)
		{
			close(clientSocket);
			break;
		}

		// Todo: 데이터 전송
		while (true)
		{
			sendLength = send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
			if (sendLength <= 0)
			{
				if (errno == EAGAIN) // non-blocking으로 인한 return인 경우
				{
					continue;
				}
				else
				{
					std::cerr << "error sending data." << std::endl;
					perror("send()");
					std::cerr << "errno: " << errno << std::endl;
					close(clientSocket);
					return EXIT_FAILURE;
				}
			}
			std::cout << "send data " << sendLength << " bytes to " << inet_ntoa(server.sin_addr)
			<< ": " << sendBuffer << std::endl;
			std::memset(sendBuffer, 0, sizeof(sendBuffer));
			break;
		}

		// // Todo: 데이터 수신
		// while (true)
		// {
		// 	recvLength = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		// 	if (recvLength <= 0)
		// 	{
		// 		if (errno == EAGAIN) // non-blocking으로 인한 return인 경우
		// 		{
		// 			continue;
		// 		}
		// 		else
		// 		{
		// 			std::cerr << "error receiving data." << std::endl;
		// 			perror("recv()");
		// 			std::cerr << "errno: " << errno << std::endl;
		// 			close(clientSocket);
		// 			return EXIT_FAILURE;
		// 		}
		// 	}
		// 	std::cout << "receive data " << recvLength << " bytes. : " << recvBuffer << std::endl;
		// 	std::memset(recvBuffer, 0, sizeof(recvBuffer));
		// 	break;
		// }
		std::cout << std::endl;
	}
	
	close(clientSocket);

	return 0;
}
