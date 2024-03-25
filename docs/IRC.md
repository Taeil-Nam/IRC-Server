# Internet Relay Chat Protocol
이 문서는 IRC Server를 구현하기 위해 RFC 1459 내용을 정리한 문서이다.  
구현을 위한 최소한의 내용만 정리되어 있으며, 더 많은 정보는 [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459)를 참고한다.  
현대의 IRC의 기능에 대해서는 [Modern IRC](https://modern.ircdocs.horse/)를 참고한다.

## 목차
- [1. 소개](#1-소개)
  - [1.1. Servers](#11-servers)
  - [1.2. Clients](#12-clients)
    - [1.2.1. Operators](#121-operators)
  - [1.3. Channels](#13-channels)
    - [1.3.1. Channel Operators](#131-channel-operators)
- [2. IRC Protocol 명세](#2-irc-protocol-명세)
  - [2.1. 개요](#21-개요)
  - [2.2. 문자 코드 (Character codes)](#22-문자-코드-character-codes)
  - [2.3. Messages](#23-messages)
    - [2.3.1. Message format in 'pseudo' BNF](#231-message-format-in-pseudo-bnf)

### 1. 소개
IRC는 텍스트 기반 회의를 위해 설계되었다.  
IRC 자체가 원격회의 시스템으로, Client-Server 모델의 분산 방식으로 운영하기에 적합하다.  
일반적으로, 단일 server를 중심으로 client(또는 다른 server)가 연결될 수 있게 하고, 필요한 메시지 전달과 같은 기능들을 수행한다.  

### 1.1. Servers
Server는 client와 다른 server에게 연결 지점을 제공한다.  
- Client에게는 다른 client와 메세지를 주고 받기 위한 연결 지점이 된다.  
- Server에게는 IRC network를 형성할 수 있는 연결 지점이 된다.  

### 1.2. Clients
Client는 서버에 연결되는 모든 것들을 말한다.  
각 client는 최대 9글자를 가지는 고유한 nickname으로 다른 client와 구별된다.  
Nickname은 아래와 같은 규칙을 따른다:
- 공백 (' '), 쉼표 (','), 별표('*'), 물음표('?'), 느낌표('!'), 골뱅이('@'), 마침표 ('.')를 포함하지 말아야한다.
- 달러 ('$'), 콜론 (':')으로 시작하면 안된다.
- Channel type('&', '#')과 같은 특정 용도를 나타내는 문자로 시작하면 안된다.  
  
모든 server는 모든 client에 대해 아래와 같은 내용을 꼭 가지고 있어야한다:  
- client가 실행 중인 호스트의 실제 이름. (the real name of the host that the client is running on)
- 해당 host의 client 사용자 이름. (the username of the client on that host)
- client가 연결된 서버. (the server to which the client is connected)

### 1.2.1. Operators
특별한 권한을 가지는 client이다.  
IRC에서 일정한 정도의 질서를 유지하지 위해 사용한다.  

### 1.3. Channels
Client들이 모여있는 그룹이다.  
동일한 channel에 있는 client들은, client가 보낸 메세지를 다 같이 수신한다.  
Channel은 맨 처음 client가 입장했을 때 암시적으로 생성된다.  
마지막 client가 channel에서 떠나면 channel은 삭제된다.  
Channel이 존재하면, 모든 client들은 channel 이름을 사용하여 해당 채널을 참조할 수 있다.  
  
Channel 이름은 '&' 또는 '#' 문자로 시작하는 최대 200자의 문자열이다.  
Channel 이름에는 공백 (' '), 제어 문자 G (ASCII 7), 쉼표 (',')를 포함할 수 없다.  
Channel은 아래와 같은 두 가지 유형으로 나뉜다:
- Regular channel : '#' 문자로 시작하며, IRC network에 속하는 모든 Server가 알고 있는 channel.
- Local channel : '&' 문자로 시작하며, 같은 server에 연결된 client만 사용 가능한 channel.
  
새로운 channel을 생성하거나, channel에 입장하려면 JOIN을 사용해야 한다.  
맨 처음 channel에 입장한 client가 channel operator가 된다.  
Channel이 이미 존재하는 경우, channel mode에 따라 해당 channel에 참여할지 여부가 결정된다.  
만약, channel이 초대 전용(+i)인 경우, 초대되지 않은 한 참여할 수 없다.  

### 1.3.1. Channel Operators
Channel operator("chop" 또는 "chanop" 이라고 부른다)는 해당 channel의 소유자로 간주된다.  
Channel operator는 아래와 같이 channel을 통제하고 조율을 유지할 수 있는 특정 권한이 부여된다:
- KICK : channel에서 client를 추방한다.
- MODE : channel의 mode를 변경한다.
- INVITE : 초대 전용 channel에 client를 초대한다.(mode + i)
- TOPIC : mode + t channel에서 channel 주제를 변경한다.
  
Channel operator는 nickname 옆에 '@' 기호로 식별된다. (NAMES, WHO, WHOIS 명령어 응답시에도 사용)

### 2. IRC Protocol 명세
### 2.1. 개요
여기에 설명된 protocol은 server-to-server, client-to-server 연결에 둘 다 사용된다.  

### 2.2. 문자 코드 (Character codes)
8비트(1옥텟)로 구성된 코드의 집합을 기반으로 사용한다.  
각 메시지는 이러한 옥텟의 임의의 수로 구성될 수 있다.  
일부 옥텟 값은 제어 코드로 사용되어 메시지 구분자로 작동한다.  
  
IRC의 스칸디나비아 출처로 인해 문자 '{', '}', '|' 는 각각 문자 '{', '}', '\\'의 소문자 변형으로 간주된다.  
이는 두 nickname의 동일성을 결정할 때 중요하다.  

### 2.3. Messages
Server와 client는 서로 메시지를 보내며 이에 대한 응답을 생성할 수도 있고 아닐 수도 있다.  
메시지에 유효한 명령이 포함되어 있는 경우, client는 지정된 대로 응답을 기대해야 하지만, 응답을 영원히 기다리는 것은 권장되지 않는다.  
Client-to-server, server-to-server 통신은 본질적으로 비동기적이다.  
  
각 IRC 메시지는 최대 세 가지 부분으로 구성될 수 있다:
- prefix(선택 사항)
- command
- command parameters(최대 15개 까지)
  
Prefix, command, command parameters는 한 개 이상의 공백(' ')으로 구분된다.  
  
Prefix의 존재는 하나의 콜론(':')을 통해 나타내며, 이는 메시지 자체의 첫 번째 문자여야 한다.
콜론과 prefix 사이에는 간격(whitespace)가 있으면 안된다.  
Prefix는 server가 메시지의 실제 원본을 표기하기 위해 사용된다.  
메세지에 prefix가 없는 경우, 해당 메시지는 메시지를 수신한 연결에서 보낸 것으로 간주된다.  
Client는 메시지를 자신이 보낼 때 prefix를 사용해서는 안되며, 만약 사용한 경우 유효한 prefix는 오직 client의 등록된 nickname 뿐이다.  
  
Command는 유효한 IRC 명령어 이거나, ASCII 텍스트로 표시된 세 자리 숫자(ex - 001, 123, 999 ...) 중 하나여야 한다.  
  
IRC 메시지는 항상 CR-LF(Carriage Return-Line Feed)쌍으로 종료되는 문자열이다.  
메시지는 CR-LF를 포함한 모든 문자를 계산하여 최대 512자까지 사용 가능하며, 길이를 초과해서는 안된다.  
즉, Command와 해당 매개 변수에 최대 510자까지 허용된다.  

### 2.3.1 Message format in 'pseudo' BNF
...