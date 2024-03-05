# 💬 ft_irc
이 프로젝트는 C++98을 사용하여 ```IRC``` (Internet Relay Chat) 서버를 만드는 프로젝트이다.

## 현재 진행도 (Progress) (20%)
- 프로젝트 세팅.
- GlobalLogger 구현.
- AssertManager 구현.
- Event 구현.
- ConsoleWindow 구현.
- Network 구현.

## 구조
```ARCHITECTURE.md``` 파일 참고.

## 기능
### TCP/IP 통신
- 소켓을 이용한 TCP/IP 통신.
- IO 멀티플렉싱을 위한 [kqueue](https://man.freebsd.org/cgi/man.cgi?kqueue) 사용.

### IRC 프로토콜
- [Internet Relay Chat (IRC)](https://datatracker.ietf.org/doc/html/rfc1459) 프로토콜 구현.
- 클라이언트에 대한 연결 등록.
- 사용자 인증, 사용자 닉네임 및 사용자 이름 관리.
- 채널 입장 및 비공개 메세지.
- 채널 운영자 및 일반 사용자.
- 채널 운영자의 명령어 구현 (KICK, INVITE, TOPIC, MODE).
- [DCC(Direct Client-to-Client)](https://modern.ircdocs.horse/dcc.html)를 이용한 파일 전송.
- GPT 챗봇.

### 서버 모니터링
- 여러 개의 모니터링 콘솔 화면 지원.
- 서버 로그 모니터링.
- 서버 상태 모니터링.

### 로깅
- 로그 파일에 메세지 로깅.
- [syslog severity](https://datatracker.ietf.org/doc/html/rfc5424#section-6.2.1)를 사용하여 메세지 분류.

## 시작하기
### 사전 요구사항
- Intel CPU Mac.
- MacOS Version = 10.15.7.
- Clang Version = 12.0.0.

### 설치
Makefile 사용을 권장한다.
1. Git clone
```bash
$> git clone <this repo>
```
2. 프로젝트 폴더로 이동  
```bash
$> cd <this repo>
```
3. "make" 명령어로 프로그램 생성  
```bash
$> make
```

### 사용법
아래와 같이 프로그램을 실행한다:
```
$> ./ircserv <port> <password>
```
- port = IRC 서버에서 사용할 포트 번호.
- password = 클라이언트 연결 등록을 위한 비밀번호.

## 프로그래밍 언어
<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> 
  <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> 
</a> 

## 라이센스
MIT 라이센스에 따라 배포된다. 자세한 내용은 ```LICENSE.txt``` 파일 참고.

## 팀 멤버
- [Taeil Nam (Tayden)](https://github.com/Taeil-Nam) - nam0314@gmail.com
- [JEEKUN PARK](https://github.com/jeekpark)
- [Ejay](https://github.com/Eonjoo-Oh)
