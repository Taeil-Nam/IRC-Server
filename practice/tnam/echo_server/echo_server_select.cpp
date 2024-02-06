// echo server

#include <iostream>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <vector>

const int bufferSize = 1000; // recv, send에 사용할 버퍼사이즈.

struct session	// 연결된 client의 정보를 저장하는 구조체
{
	sockaddr_in addr;
	int socket;
	char recvBuffer[bufferSize];
	int recvBytes;
	int sendBytes;
};

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

	// 4. select를 이용한 IO multiplexing
	// multiplexing을 이용하면 아래와 같은 문제를 해결할 수 있다.
	// - blocking socket : block이 되는 상황을 방지할 수 있음.
	// - non-blocking socket : 무한 루프를 돌면서 계속 반복적으로 확인하는 상황을 방지할 수 있음.
	// 즉, multiplexing은 blocking, non-blocking 어느 곳에든 사용 가능하다.

	//	void
	//  FD_CLR(fd, fd_set *fdset);
	//	fd를 fdset에서 제거한다.
	//
	//  int
	//  FD_ISSET(fd, fd_set *fdset);
	//	fd가 fdset에 있는지 알려준다.
	//
	//  void
	//  FD_SET(fd, fd_set *fdset);
	//	fd를 fdset에 추가한다.
	//
	//  void
	//  FD_ZERO(fd_set *fdset);
	//	fdset을 초기화한다.
	//
	//  int
	//  select(int nfds,
	//		fd_set *restrict readfds,
	//      fd_set *restrict writefds,
	//      fd_set *restrict errorfds,
	//      struct timeval *restrict timeout);

	std::vector<session> sessions;	// client 세션들을 모아놓는 vector.
	sessions.reserve(100);

	fd_set reads;	// read를 위한 fd들을 담아놓을 fd_set.
	fd_set writes;	// write를 위한 fd들을 담아놓은 fd_set.

	while (true)
	{
		// fd_set 초기화.
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		// serverSocket(ListenSocket) 등록.
		FD_SET(serverSocket, &reads);	// serverSocket은 read를 위한 소켓.

		// clientSocket 등록.
		for (int i = 0; i < sessions.size(); i++)
		{
			if (sessions[i].recvBytes <= sessions[i].sendBytes)
			{
				FD_SET(sessions[i].socket, &reads);
			}
			else
			{
				FD_SET(sessions[i].socket, &writes);
			}
		}

		// select()로 read or write가 준비된 소켓이 있는지 확인.
		int readySockets = select(FD_SETSIZE, &reads, &writes, NULL, NULL);
		if (readySockets == -1)
		{
			perror("select()");
			std::cerr << "errno: " << errno << std::endl;
			break;
		}
		// select() 호출이 완료되면, 준비된 소켓 이외의 소켓들은 fd_set에서 삭제된다.
		
		// serverSocket(ListenSocket) 확인.
		if (FD_ISSET(serverSocket, &reads))
		{
			session client;
			socklen_t clientAddrLength = sizeof(client.addr);
			int clientSocket = accept(serverSocket,
				reinterpret_cast<sockaddr*>(&client.addr), &clientAddrLength);
			if (clientSocket != -1)	// client 정상 연결시
			{
				std::cout << "Client(" << inet_ntoa(client.addr.sin_addr) << ") connected." << std::endl;
				std::cout << std::endl;
				client.socket = clientSocket;
				client.recvBytes = 0;
				client.sendBytes = 0;
				std::memset(&client.recvBuffer, 0, sizeof(client.recvBuffer));
				sessions.push_back(client); // client 세션 만들어서 sessions에 추가.
			}
			else
			{
				perror("accept()");
				std::cerr << "errno: " << errno << std::endl;
			}
		}
		
		// clientSocket 확인.
		for (int i = 0; i < sessions.size(); i++)
		{
			// Read
			if (FD_ISSET(sessions[i].socket, &reads))
			{
				int recvLen = recv(sessions[i].socket, sessions[i].recvBuffer, sizeof(sessions[i].recvBuffer), 0);
				if (recvLen == 0) // 상대방과 연결이 끊긴 경우.
				{
					std::cout << "Client(" << inet_ntoa(sessions[i].addr.sin_addr) << ") disconnected." << std::endl;
					std::cout << std::endl;
					close(sessions[i].socket);
					sessions.erase(sessions.begin() + i); // 해당 세션 삭제.
					continue;
				}
				else if (recvLen == -1) // 오류가 발생한 경우.
				{
					perror("recv()");
					std::cerr << "errno: " << errno << std::endl;
					close(sessions[i].socket);
					sessions.erase(sessions.begin() + i); // 해당 세션 삭제.
					continue;
				}
				else
				{
					sessions[i].recvBytes = recvLen;
					std::cout << "receive data " << recvLen << " bytes from " << inet_ntoa(sessions[i].addr.sin_addr)
					<< ": " << sessions[i].recvBuffer << std::endl;
				}
			}
			// Write
			if (FD_ISSET(sessions[i].socket, &writes))
			{
				int sendLen = send(sessions[i].socket,
					&sessions[i].recvBuffer[sessions[i].sendBytes], sessions[i].recvBytes - sessions[i].sendBytes, 0);
					// send() 호출시, 100 bytes 크기를 보낸다고 호출해도, 커널 sendBuffer가 모자랄 경우 실제로 100 bytes보다 작은 크기(50 bytes)만 보내게 될 수도 있음.
					// 그럴 경우, 다음 send() 호출시 이전에 보냈던 buffer의 왼쪽 부분은 제외하고, 보내지 못했던 부분부터 보내도록 이와 같은 코드를 사용한다.
				if (sendLen == -1) // send 실패시.
				{
					perror("send()");
					std::cerr << "errno: " << errno << std::endl;
					close(sessions[i].socket);
					sessions.erase(sessions.begin() + i); // 해당 세션 삭제.
					continue;
				}
				else
				{
					std::cout << "send data " << sendLen << " bytes to " << inet_ntoa(sessions[i].addr.sin_addr)
					<< ": " << sessions[i].recvBuffer << std::endl;
					sessions[i].sendBytes += sendLen;
					if (sessions[i].recvBytes == sessions[i].sendBytes)
					{
						sessions[i].recvBytes = 0;
						sessions[i].sendBytes = 0;
					}
				}
			}
		}
	}
	close(serverSocket);

	return 0;
}
