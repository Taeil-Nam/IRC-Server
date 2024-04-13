# 💬 GameRC 프로젝트
이 프로젝트는 C++98을 사용하여 `IRC` (Internet Relay Chat) 서버를 만드는 프로젝트이다.

---

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

### 동적 라이브러리
BSD 계열 시스템에서 자주 사용될 기능을 동적 라이브러리로 구현:
- Display : 콘솔 입출력.
- Network : 네트워크.
- Event : IO 멀티플렉싱.
- Logger : 로깅.
- Assert : 커스텀 Assert.

---

## 시작하기
### 사전 요구사항
- Intel CPU Mac.
- MacOS Version = 10.15.7.
- Clang Version = 12.0.0.

### 설치
1. Git clone
```bash
$> git clone [this repo]
```
2. 프로젝트 폴더로 이동  
```bash
$> cd [this repo]
```
3. "make lib" 명령어로 동적 라이브러리 생성  
```bash
$> make lib
```
4. "make" 명령어로 프로그램 생성  
```bash
$> make
```

### 사용법
아래와 같이 프로그램을 실행한다:
```bash
$> ./ircserv [port] [password]
```
- port = IRC 서버에서 사용할 포트 번호.
- password = 클라이언트 연결 등록을 위한 비밀번호.

---

## 프로그래밍 언어
<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> 
  <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> 
</a>

---

## 라이센스
이 프로그램은 MIT 라이센스에 따라 라이센스가 부여된다:

Copyright © 2024 Game Development Forever.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

## 팀 멤버
- [Taeil Nam (Tayden)](https://github.com/Taeil-Nam) - nam0314@gmail.com
- [JEEKUN PARK](https://github.com/jeekpark) - jeekunp@naver.com
- [Ejay](https://github.com/Eonjoo-Oh)
