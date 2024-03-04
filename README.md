# 💬 ft_irc
이 프로젝트는 C++98을 사용하여 ```IRC``` (Internet Relay Chat) 서버를 만드는 프로젝트이다.  
(This project involves creating an ```IRC``` (Internet Relay Chat) server using C++98).

[소프트웨어 구조](https://github.com/Taeil-Nam/ft_irc/blob/main/ARCHITECTURE.md)  
([Software Architecture](https://github.com/Taeil-Nam/ft_irc/blob/main/ARCHITECTURE.md))

## 현재 진행도 (Progress) (20%)
- 프로젝트 세팅.
- GlobalLogger 구현.
- AssertManager 구현.
- Event 구현.
- ConsoleWindow 구현.
- Network 구현.

## 기능 (Features)
### TCP/IP 통신 (TCP/IP Communication)
- 소켓을 이용한 TCP/IP 통신.  
(TCP/IP communication using socket.)
- IO 멀티플렉싱을 위한 [kqueue](https://man.freebsd.org/cgi/man.cgi?kqueue) 사용.  
(Used [kqueue](https://man.freebsd.org/cgi/man.cgi?kqueue) for IO Multiplexing.)

### IRC 프로토콜 (IRC Protocol)
- [Internet Relay Chat (IRC)](https://datatracker.ietf.org/doc/html/rfc1459) 프로토콜 구현.  
(Implementation [Internet Relay Chat (IRC)](https://datatracker.ietf.org/doc/html/rfc1459) protocol.)
- 클라이언트에 대한 연결 등록.  
(Connection registration for client.)
- 사용자 인증, 사용자 닉네임 및 사용자 이름 관리.  
(User authentication, nickname and username management.)
- 채널 입장 및 비공개 메세지.  
(Channel joining and private messaging.)
- 채널 운영자 및 일반 사용자.  
(Channel operators and regular users.)
- 채널 운영자의 명령어 구현 (KICK, INVITE, TOPIC, MODE).  
(Implementation of channel operators commands like KICK, INVITE, TOPIC, MODE.)
- [DCC(Direct Client-to-Client)](https://modern.ircdocs.horse/dcc.html)를 이용한 파일 전송.  
(File transffering using [DCC(Direct Client-to-Client)](https://modern.ircdocs.horse/dcc.html).)
- GPT 챗봇.  
(GPT chatbot.)

### 서버 모니터링 (Server Monitoring)
- 여러 개의 모니터링 콘솔 화면 지원.  
(Supports multiple monitoring console windows.)
- 서버 로그 모니터링.  
(Server log monitoring.)
- 서버 상태 모니터링.  
(Server status monitoring.)

### 로깅 (Logging)
- 로그 파일에 메세지 로깅.  
(Logging messages to a log file.)
- [syslog severity](https://datatracker.ietf.org/doc/html/rfc5424#section-6.2.1)를 사용하여 메세지 분류.  
(Categorizing log messages using [syslog severity](https://datatracker.ietf.org/doc/html/rfc5424#section-6.2.1).)

## 시작하기 (Getting Started)
### 전제 조건 (Prerequisites)
- Intel CPU Mac.
- MacOS Version = 10.15.7.
- Clang Version = 12.0.0.

### 설치 (Installation)
Makefile 사용을 권장한다.  
(Recommand to using Makefile)
1. Git clone
```
$> git clone <this repo>
```
2. 프로젝트 폴더로 이동  
(Move to project directory)
```
$> cd <this repo>
```
3. "make" 명령어로 프로그램 생성  
(Use "make" command to create program)
```
$> make
```

### 사용법 (Usage)
아래와 같이 프로그램을 실행한다:  
(Execute program as below):
```
$> ./ircserv <port> <password>
```
- port = IRC 서버에서 사용할 포트 번호.  
(port number for IRC server.)
- password = 클라이언트 연결 등록을 위한 비밀번호.  
(password to connection registration for client.)

## 프로그래밍 언어 (Programming languages)
<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> 
  <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> 
</a> 

## 라이센스 (License)
MIT 라이센스에 따라 배포된다. 자세한 내용은 ```LICENSE.txt``` 파일 참고.  
(Distributed under the MIT License. See ```LICENSE.txt``` for more information.)

## 팀원 (Team members)
- [Taeil Nam (Tayden)](https://github.com/Taeil-Nam) - nam0314@gmail.com
- [JEEKUN PARK](https://github.com/jeekpark)
- [Ejay](https://github.com/Eonjoo-Oh)
