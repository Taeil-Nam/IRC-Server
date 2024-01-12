## Echo server-client
Echo server는, client가 전송한 데이터를 그대로 돌려주는 server를 말합니다.  
간단한 Echo server, client 구현을 통해 소켓 프로그래밍 연습을 해봐요.

### 환경
- OS : macOS 10.15.7.
- Language : c++98.
- Compiler : Apple clang version 12.0.0 (clang-1200.0.32.28).

### 사용법
1. "./server" 명령어로 echo server 실행.
2. "./client 127.0.0.1" 명령어로 echo client 실행.
3. client에서 전송할 메세지 입력 후, server로 부터 다시 전달 받는지 확인.

### 추가 정보
- Echo server에 사용된 port number = 194.
- client 실행에 사용되는 인자 = server의 IP 주소.
  - 현재 server가 동일한 컴퓨터에 있기때문에, 자기 자신의 IP를 나타내는 "127.0.0.1"로 접속이 가능합니다.
