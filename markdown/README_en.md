# 💬 GameRC Project
This project involves creating an ```IRC``` (Internet Relay Chat) server using C++98.

---

## Architecture
See [GameRC Architecture](https://taeil-nam.github.io/ft_irc/).

---

## Features
### TCP/IP Communication
- TCP/IP communication using socket.
- Used [kqueue](https://man.freebsd.org/cgi/man.cgi?kqueue) for IO Multiplexing.

### IRC Protocol
- Implementation [Internet Relay Chat (IRC)](https://datatracker.ietf.org/doc/html/rfc1459) protocol.
- Connection registration for client.
- User authentication, nickname and username management.
- Channel joining and private messaging.
- Channel operators and regular users.
- Implementation of channel operators commands like KICK, INVITE, TOPIC, MODE.
- File transffering using [DCC(Direct Client-to-Client)](https://modern.ircdocs.horse/dcc.html).
- Russian roulette chatbot.

### Server Monitoring
- Supports multiple monitoring console windows.
- Server log monitoring.
- Server status monitoring.

### Logging
- Logging messages to a log file.
- Categorizing log messages using [syslog severity](https://datatracker.ietf.org/doc/html/rfc5424#section-6.2.1).

### Dynamic Library
Implement frequently used features as a dynamic library in BSD-based systems:
- Display: Console input/output.
- Network: Networking.
- Event: IO multiplexing.
- Logger: Logging.
- Assert: Custom Assert.

---

## Getting Started
### Prerequisites
- Intel CPU Mac.
- MacOS Version = 10.15.7.
- Clang Version = 12.0.0.

### Installation
1. Git clone
```bash
$> git clone [this repo]
```
2. Move to project directory
```bash
$> cd [this repo]
```
3. Use "make lib" command to create Dynamic Library  
```bash
$> make lib
```
4. Use "make" command to create program
```bash
$> make
```

### Usage
Execute program as below:
```bash
$> ./ircserv [port] [password]
```
- port = port number for IRC server.
- password = password to connection registration for client.

---

## Programming languages
<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> 
  <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> 
</a> 

---

## License
Distributed under MIT license:

Copyright © 2024 Game Development Forever.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

## Team members
- [Taeil Nam (Tayden)](https://github.com/Taeil-Nam) - nam0314@gmail.com
- [JEEKUN PARK](https://github.com/jeekpark) - jeekunp@naver.com
- [Ejay](https://github.com/Eonjoo-Oh)
