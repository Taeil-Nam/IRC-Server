# 💬 ft_irc 
This project involves creating an ```IRC``` (Internet Relay Chat) server using C++98.

## Architecture
See ```ARCHITECTURE.md```.

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
- GPT chatbot.

### Server Monitoring
- Supports multiple monitoring console windows.
- Server log monitoring.
- Server status monitoring.

### Logging
- Logging messages to a log file.
- Categorizing log messages using [syslog severity](https://datatracker.ietf.org/doc/html/rfc5424#section-6.2.1).

## Getting Started
### Prerequisites
- Intel CPU Mac.
- MacOS Version = 10.15.7.
- Clang Version = 12.0.0.

### Installation
Recommand to using Makefile
1. Git clone
```bash
$> git clone [this repo]
```
2. Move to project directory
```bash
$> cd [this repo]
```
3. Use "make" command to create program
```bash
$> make
```

### Usage
Execute program as below:
```
$> ./ircserv [port] [password]
```
- port = port number for IRC server.
- password = password to connection registration for client.

## Programming languages
<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> 
  <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> 
</a> 

## License
Distributed under the MIT License. See ```LICENSE.txt``` for more information.

## Team members
- [Taeil Nam (Tayden)](https://github.com/Taeil-Nam) - nam0314@gmail.com
- [JEEKUN PARK](https://github.com/jeekpark)
- [Ejay](https://github.com/Eonjoo-Oh)
