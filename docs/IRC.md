# Internet Relay Chat Protocol
이 문서는 IRC Server를 구현하기 위해 RFC 1459 내용을 정리한 문서이다.  
구현을 위한 최소한의 내용만 정리되어 있으며, 더 많은 정보는 [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459)를 참고한다.  
현대의 IRC의 기능에 대해서는 [Modern IRC](https://modern.ircdocs.horse/)를 참고하였다.

---

# 목차
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
  - [2.4. Numeric replies](#24-numeric-replies)
- [3. IRC Concepts](#3-irc-concepts)
  - [3.1. One-to-one communication](#31-one-to-one-communication)
  - [3.2. One-to-many](#32-one-to-many)
    - [3.2.2. To a group (channel)](#322-to-a-group-channel)
- [4. Message details](#4-message-details)
  - [4.1. Connection Registration](#41-connection-registration)
    - [4.1.1. Password message](#411-password-message)
    - [4.1.2. Nick message](#412-nick-message)
    - [4.1.3. User message](#413-user-message)
    - [4.1.6. Quit](#416-quit)
  - [4.2. Channel operations](#42-channel-operations)
    - [4.2.1. Join message](#421-join-message)
    - [4.2.2. Part message](#422-part-message)
    - [4.2.3. Mode message](#423-mode-message)
      - [4.2.3.1. Channel modes](#4231-channel-modes)
    - [4.2.4. Topic message](#424-topic-message)
    - [4.2.7. Invite message](#427-invite-message)
    - [4.2.8. Kick command](#428-kick-command)
  - [4.4. Sending messages](#44-sending-messages)
    - [4.4.1. Private messages](#441-private-messages)
    - [4.4.2. Notice](#442-notice)
  - [4.6 Miscellaneous messages](#46-miscellaneous-messages)
    - [4.6.2. Ping message](#462-ping-message)
    - [4.6.3. Pong message](#463-pong-message)
  - [6. REPLIES](#6-replies)
    - [6.1 Error Replies](#61-error-replies)
    - [6.2. Command responses](#62-command-responses)
---

## 1. 소개
IRC는 텍스트 기반 회의를 위해 설계되었다.  
IRC 자체가 원격회의 시스템으로, Client-Server 모델의 분산 방식으로 운영하기에 적합하다.  
일반적으로, 단일 server를 중심으로 client(또는 다른 server)가 연결될 수 있게 하고, 필요한 메시지 전달과 같은 기능들을 수행한다.

---

### 1.1. Servers
Server는 client와 다른 server에게 연결 지점을 제공한다.  
- Client에게는 다른 client와 메세지를 주고 받기 위한 연결 지점이 된다.  
- Server에게는 IRC network를 형성할 수 있는 연결 지점이 된다.

---

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

#### 1.2.1. Operators
특별한 권한을 가지는 client이다.  
IRC에서 일정한 정도의 질서를 유지하지 위해 사용한다.  

---

### 1.3. Channels
Client들이 모여있는 그룹이다.  
동일한 channel에 있는 client들은, client가 보낸 메세지를 다 같이 수신한다.  
Channel은 맨 처음 client가 입장했을 때 암시적으로 생성된다.  
마지막 client가 channel에서 떠나면 channel은 삭제된다.  
Channel이 존재하면, 모든 client들은 channel 이름을 사용하여 해당 채널을 참조할 수 있다.  
  
Channel 이름은 `'&'` 또는 `'#'` 문자로 시작하는 최대 200자의 문자열이다.  
Channel 이름에는 공백 (' '), 제어 문자 G (ASCII 7), 쉼표 (',')를 포함할 수 없다.  
Channel은 아래와 같은 두 가지 유형으로 나뉜다:
- Regular channel : `'#'` 문자로 시작하며, IRC network에 속하는 모든 Server가 알고 있는 channel.
- Local channel : `'&'` 문자로 시작하며, 같은 server에 연결된 client만 사용 가능한 channel.
  
새로운 channel을 생성하거나, channel에 입장하려면 JOIN을 사용해야 한다.  
맨 처음 channel에 입장한 client가 channel operator가 된다.  
Channel이 이미 존재하는 경우, channel mode에 따라 해당 channel에 참여할지 여부가 결정된다.  
만약, channel이 초대 전용(+i)인 경우, 초대되지 않은 한 참여할 수 없다.

#### 1.3.1. Channel Operators
Channel operator("chop" 또는 "chanop" 이라고 부른다)는 해당 channel의 소유자로 간주된다.  
Channel operator는 아래와 같이 channel을 통제하고 조율을 유지할 수 있는 특정 권한이 부여된다:
- KICK : channel에서 client를 추방한다.
- MODE : channel의 mode를 변경한다.
- INVITE : 초대 전용 channel에 client를 초대한다.(mode + i)
- TOPIC : mode + t channel에서 channel 주제를 변경한다.
  
Channel operator는 nickname 옆에 '@' 기호로 식별된다. (NAMES, WHO, WHOIS 명령어 응답시에도 사용)

---

## 2. IRC Protocol 명세
### 2.1. 개요
여기에 설명된 protocol은 server-to-server, client-to-server 연결에 둘 다 사용된다.

---

### 2.2. 문자 코드 (Character codes)
8비트(1옥텟)로 구성된 코드의 집합을 기반으로 사용한다.  
각 메시지는 이러한 옥텟의 임의의 수로 구성될 수 있다.  
일부 옥텟 값은 제어 코드로 사용되어 메시지 구분자로 작동한다.  
  
IRC의 스칸디나비아 출처로 인해 문자 `'{', '}', '|'` 는 각각 문자 `'{', '}', '\'`의 소문자 변형으로 간주된다.  
이는 두 nickname의 동일성을 결정할 때 중요하다.

---

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

#### 2.3.1. Message format in 'pseudo' BNF
protocol 메시지는 CR-LF 두 문자의 쌍으로 구분된다.  
추출된 메시지는 `<prefix>` | `<command>` | `<middle> 또는 <trailing>에 매칭되는 parameter list`로 파싱된다.  
이를 위한 BNF 표현은 아래와 같다:

```
<message>   ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
<prefix>    ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
<command>   ::= <letter> { <letter> } | <number> <number> <number>
<SPACE>     ::= ' ' { ' ' }
<params>    ::= <SPACE> [ ':' <trailing> | <middle> <params> ]

<middle>    ::= <SPACE나 NUL, CR, LF를 포함하지 않는 비어있지 않은 옥텟의 시퀀스, 첫 번째는 ':'이 될 수 없음>
<trailing>  ::= <NUL, CR, LF를 포함하지 않는, 가능하면 비어있는 옥텟의 시퀀스>

<crlf>      ::= CR LF
```

NOTES:
1. `<SPACE>`는 오직 공백 문자(0x20)로만 구성된다. 탭이나 모든 다른 제어 문자는 공백이 아닌 것으로 간주된다.
2. Parameter list를 추출한 후에는, `<middle>`이나 `<trailing>`에 의해 매치된 모든 parameter가 동일하다.  
  `<trailing>`은 parameter 내부에 공백을 허용하는 구문적 트릭에 불과하다.
3. CR과 LF가 매개변수 문자열에 나타날 수 없다는 사실은 단지 메시지 구조의 부산물일 뿐이다. 이것은 나중에 변경될 수 있다.
4. NUL 문자는 추가적은 복잡성을 유발할 수 있기 때문에, 메시지 내부에 허용되지 않는다.
5. 마지막 parameter는 빈 문자열일 수 있다.
6. 확장된 prefix`(['!' <user> ] ['@' <host> ])`의 사용은 server-to-server 통신에서는 사용되지 않아야 하며, 메세지가 누구로부터 왔는지에 대해 추가적인 질의 없이 client에게 알려주도록 의도된 것이다.
  
대부분의 프로토콜 메시지는 목록에서의 위치에 따라 지정된 추가 의미론과 문법을 매개변수 문자열에 지정한다.  
예를 들어, 많은 서버 명령은 명령 다음의 첫 번째 매개변수가 대상 목록이라고 가정한다.  이는 다음과 같이 설명될 수 있다:
```
<target> ::= <to> [ "," <target> ]
<to> ::= <channel> | <user> '@' <servername> | <nick> | <mask>
<channel> ::= ('#' | '&') <chstring>
<servername> ::= <host>
<host> ::= 호스트명에 대한 허용 사항은 RFC 952 [DNS:4]를 참조
<nick> ::= <letter> { <letter> | <number> | <special> }
<mask> ::= ('#' | '$') <chstring>
<chstring> ::= <any 8bit code except SPACE, BELL, NUL, CR, LF and comma (',')>
```

다른 매개 변수 문법은 다음과 같다:
```
<user> ::= <nonwhite> { <nonwhite> }
<letter> ::= 'a' ... 'z' | 'A' ... 'Z'
<number> ::= '0' ... '9'
<special> ::= '-' | '[' | ']' | '' | '`' | '^' | '{' | '}'
<nonwhite> ::= <any 8bit code except SPACE (0x20), NUL (0x0), CR (0xd), and LF (0xa)>
```

---

### 2.4. Numeric replies
Server로 보내는 대부분의 메시지는 어떤 형태의 응답을 생성한다.  
가장 일반적인 응답은, error와 정상 답변 모두에 사용되는 숫자형 응답(Numeric replies)이다.  
숫자형 응답은 발신자 prefix, 세 자리 숫자, 응답의 대상으로 이루어진 하나의 메시지로 보내져야 한다.  
다양한 응답의 목록은 section 6에서 제공된다.

---

## 3. IRC Concepts
이 섹션에서는 IRC Protocol의 실제 개념과, 다른 종류의 메시지를 어떻게 전달하는지에 대해 설명한다.

```
                          1--\
                              A        D---4
                          2--/ \      /
                                B----C
                               /      \
                              3        E

   Servers: A, B, C, D, E         Clients: 1, 2, 3, 4

                    [ 그림. 2. Sample small IRC network ]
```

---

### 3.1. One-to-one communication
Client간 1:1 통신을 말한다.  
Client가 서로 안전하게 대화할 수 있도록 모든 server가 최단 경로로 메시지를 전달해야한다.  
다음 예시들은 위의 그림2를 참고한다:
- Example 1:  
Client 1, 2 사이의 메시지는 오직 server A만 볼 수 있으며, 이는 직접 Client 2로 메시지를 보낸다.
- Example 2:  
Client 2, 3 사이의 메시지는 server A, B와 client 3이 볼 수 있다.
- Example 3:  
Client 2, 4 사이의 메시지는 server A, B, C, D와 client 4가 볼 수 있다.

---

### 3.2. One-to-Many

---

#### 3.2.2. To a group (channel)
IRC에서 channel은 multicast 그룹과 동등한 역할을 한다.  
Channel은, 사람들이 channel에 참여하고 떠남에 따라 생성되고 사라지는 동적인 존재다.  
Channel에서 진행되는 대화는 해당 channel에서 활동하는 사용자를 지원하는 server에만 전송된다.  
같은 channel에 여러 사용자가 있는 경우, 메시지는 해당 server로 한 번만 전송된 후 각 client에게 전달된다.  

다음 예시들은 위의 그림2를 참고한다:
- Example 4:  
Channel에 1명의 client가 있는 경우. channel로 보내진 메시지는 server로 가고 그 후에는 어디로도 가지 않는다.
- Example 5:  
Channel에 2명의 client가 있는 경우. 모든 메시지는 두 client 간의 개인 메시지인 것처럼 경로를 따라 전송된다.
- Example 6:  
Channel에 client 1, 2, 3이 있는 경우. channel로 보내진 모든 메시지는 메시지가 지나야 하는 모든 client와 server에게 전송된다. client 1이 메시지를 보내면, 그 메시지는 client 2에게 전달되고 server B를 통해 client 3에게 전달된다.

---

## 4. Message details
이 섹션에서는 IRC server와 client가 인식하는 각 메시지를 설명한다.  
  
Client가 연결된 server는 메시지를 분석하여 적절한 error을 반환해야 한다.  
메시지 분석 중 fatal error가 발생하면, client에게 error를 보내고 분석을 중단해야 한다.  
Fatal error는 잘못된 command, server가 아직 알지 못하는 목적지(server, nick, channel 이름 등), 충분하지 않은 parameter, 잘못된 권한 등으로 발생할 수 있다.  
  
완전한 parameter가 제시된 경우, 각각의 유효성을 검사하고 client에게 적절한 응답을 보내야 한다.  
쉼표(',')를 구분자로 parameter 목록을 사용하는 경우, 각 항목에 대해 응답을 보내야 한다.  
  
일부 메시지는 아래 예시와 같이 전체 형식을 사용한다:
`:Name COMMAND parameter list`

---

### 4.1. Connection Registration
여기서 설명하는 Command들은, client 또는 server로서 IRC server와 연결을 등록하고 해제하는데 사용된다.  
  
Client나 Server 연결이 등록되는데 `PASS` command가 필요하지 않지만, 모든 server 연결에 비밀번호가 있어야 일정 수준의 보안을 제공할 수 있기 때문에 사용이 권장된다.  
권장되는 client 연결 순서는 다음과 같다:
1. Pass message
2. Nick message
3. User message

---

#### 4.1.1. Password message
```
   Command: PASS
Parameters: <password>

Numeric replies:
  ERR_NEEDMOREPARAMS
  ERR_ALREADYREGISTRED

Example:
  PASS secretpasswordhere
```
PASS command는 "연결 비밀번호"를 설정하는데 사용된다.  
비밀번호는 연결을 등록하기 전에 설정되어야 한다.  
이를 위해 client는 NICK/USER 조합을 보내기 전에 PASS command를 보내야 하고, server는 무조건 어떠한 SERVER command 보다 PASS command를 먼저 보내야 한다.  
  
제공된 비밀번호는 server, client간 일치해야 한다.  
비밀번호 등록 전에 PASS command를 여러 개 보낼 수 있지만, 등록되는데 사용되는 것은 마지막으로 보낸 것 뿐이며, 등록된 후에는 변경할 수 없다.

---

#### 4.1.2. Nick message
```
   Command: NICK
Parameters: <nickname> [ <hopcount> ]

Numeric replies:
  ERR_NONICKNAMEGIVEN
  ERR_ERRONEUSNICKNAME
  ERR_NICKNAMEINUSE
  ERR_NICKCOLLISION

Example:
  NICK Wiz              ; 새로운 닉네임 "Wiz" 설정.
```
NICK 메시지는 사용자에게 닉네임을 부여하거나 이전 닉네임을 변경하는데 사용된다.  
  
닉네임 충돌이 발생하는 경우 해당 닉네임이 서버의 데이터베이스에서 제거되며, 다른 server의 데이터베이스에서 해당 닉네임을 제거하기 위해 KILL 명령을 전달한다.  
닉네임 충돌을 일으킨 NICK 메시지가 닉네임 변경이었다면, 이전 닉네임도 제거되어야 한다.  
  
Server가 직접 연결된 client로 부터 동일한 NICK을 받으면 ERR_NICKCOLLISION을 client에게 전송하며, NICK 명령을 무시하고 어떠한 KILL도 생성하지 않는다.  

---

#### 4.1.3. User message
```
   Command: USER
Parameters: <username> <hostname> <servername> <realname>

Numeric replies:
  ERR_NEEDMOREPARAMS
  ERR_ALREADYREGISTRED

Example:
  USER guest tolmoon tolsun :Ronnie Reagan
```
USER 메시지는 연결의 시작에서 새로운 사용자의 username, hostname, servername, realname을 지정하는데 사용된다.  
일반적으로 직접 연결된 client로부터 USER 명령이 오는 경우에는 hostname과 servername이 보안상의 이유로 무시된다.  
  
realname 매개변수는 마지막 매개변수여야 하며, 공백 문자를 포함할 수 있으므로 앞에 콜론(':')을 붙여야 인식 가능하다.  

---

#### 4.1.6. Quit
```
   Command: Quit
Parameters: [<Quit message>]

Numeric replies:
  None

Example:
  QUIT :Gone to have lunch        ; Preferred message format.
```
Quit message와 함께 client 세션이 종료된다.  
Server는 quit message를 보낸 client와 연결을 종료해야 한다.  
"Quit Message"가 주어진 경우, 기본 메시지인 nickname을 대신해서 전송된다.  

---

### 4.2. Channel operations
이 섹션은 channel을 조작하는데 관련되어 있다.  

---

#### 4.2.1. Join message
```
   Command: JOIN
Parameters: <channel>{,<channel>} [<key>{,<key>}]

Numeric replies:
  ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
  ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
  ERR_CHANNELISFULL               ERR_BADCHANMASK
  ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
  RPL_TOPIC

Example:
  JOIN #foobar                    ; #foobar 채널에 입장.

  JOIN &foo fubar                 ; &foo 채널에 "fubar" 키를 사용하여 입장.

  JOIN #foo,&bar fubar            ; #foo 채널에 "fubar" 키를 사용하여 입장하고, &bar는 키를 사용하지 않음.

  JOIN #foo,#bar fubar,foobar     ; #foo 채널에 "fubar" 키를 사용하여 입장하고, #bar 채널에 "foobar" 키를 사용하여 입장.

  JOIN #foo,#bar                  ; #foo, #bar 채널에 입장.
```
JOIN 명령은 client가 특정 channel에 입장하는데 사용된다.  
Client가 channel에 참여할 수 있는지 여부는 client가 연결된 server만 확인한다.  
  
JOIN에 대한 조건은 다음과 같다:
1. 초대 전용 channel인 경우, 사용자는 초대를 받아야 한다.
2. 사용자의 nickname/username/hostname이 ban 목록과 일치하지 않아야 한다.
3. 비밀번호가 설정된 경우, 올바른 비밀번호가 제공되어야 한다.
  
이러한 사항은 MODE 명령에서 자세히 설명된다 ([Section 4.2.3](#423-mode-message) 참조).
  
사용자가 channel에 참여하면 해당 server가 channel에 미치는 모든 명령에 대한 알림을 받는다.  
이에는 MODE, KICK, PART, QUIT, PRIVMSG/NOTICE가 포함된다.  
  
JOIN이 성공하면 사용자에게 채널의 주제(PRL_TOPIC 사용) 및 channel에 있는 사용자 목록(RPL_NAMREPLY)이 전송된다.  
이 목록에는 해당 사용자도 포함되어야 한다.

---

#### 4.2.2. Part message
```
   Command: PART
Parameters: <channel>{,<channel>}

Numeric replies:
  ERR_NEEDMOREPARAMS
  ERR_NOSUCHCHANNEL
  ERR_NOTONCHANNEL

Example:
  PART #twilight_zone             ; "#twilight_zone" 채널 떠남.

  PART #oz-ops,&group5            ; "&group5", "#oz-ops" 채널 떠남.
```
Part 메시지는 해당 메시지를 보낸 client를 매개 변수 문자열에 나열된 channel에서 제거하기 위해 사용된다.  

---

#### 4.2.3. Mode message
```
   Command: MODE
```
Username과 channel의 mode를 변경할 수 있는 command이다.  
MODE 메시지를 파싱할 때, 전체 메시지를 먼저 파싱 후 변경 사항을 적용하는 것이 권장된다.

---

##### 4.2.3.1. Channel modes
```
Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>]
```
MODE command는 channel operator가 "그들의" channel 특성을 변경할 수 있도록 제공된다.  
또한 channel operator가 생성될 수 있도록 server가 channel mode를 변경할 수 있어야 한다.  
  
Channel에 사용 가능한 mode는 다음과 같다:
```
o - 채널 운영자 권한을 부여하거나 회수한다.
p - 비공개 채널 플래그.
s - 비밀 채널 플래그.
i - 초대 전용 채널 플래그.
t - 채널 주제를 설정할 수 있는 권한을 채널 운영자에게만 부여한다.
n - 외부 클라이언트에서 채널로의 메시지를 허용하지 않습니다.
m - 모더레이트된 채널.
l - 채널의 사용자 제한을 설정한다.
b - 사용자를 차단하기 위한 차단 마스크를 설정한다.
v - 모더레이트된 채널에서 발언할 수 있는 권한을 부여하거나 회수한다.
k - 채널 키(암호)를 설정한다.
```
'o' 및 'b' 옵션을 사용할 때, 모드 명령 당 총 세 개의 제한이 설정되어 있다.  
즉, 'o'와 'b'의 임의의 조합은 한 번의 MODE 명령에서 최대 세 번까지만 사용할 수 있다.

---

#### 4.2.4. Topic message
```
   Command: TOPIC
Parameters: <channel> [<topic>]

Numeric Replies:
  ERR_NEEDMOREPARAMS
  ERR_NOTONCHANNEL
  RPL_NOTOPIC
  RPL_TOPIC
  ERR_CHANOPRIVSNEEDED

Examples:
  TOPIC #test :another topic      ; #test 채널의 topic을 "another topic"으로 설정.

  TOPIC #test                     ; #test 채널의 topic을 확인.
```
TOPIC 메시지는 channel의 topic을 변경하거나 확인하기 위해 사용한다.  
만약 `<topic>`이 명시되지 않은 경우, `<channel>`의 topic이 반환된다.  
`<topic>`이 parameter로 명시된 경우, channel의 mode가 이를 허용한다면, `<channel>`의 topic을 변경한다.

---

#### 4.2.7. Invite message
```
   Command: INVITE
Parameters: <nickname> <channel>

Numeric Replies:
  ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
  ERR_NOTONCHANNEL                ERR_USERONCHANNEL
  ERR_CHANOPRIVSNEEDED
  RPL_INVITING                    RPL_AWAY

Examples:
  INVITE Wiz #Twilight_Zone       ; "Wiz"를 #Twilight_Zone 채널에 초대.
```
INVITE 메시지는 사용자를 channel로 초대하는 데 사용된다.  
`<nickname>`매개변수는 `<channel>`에 초대될 사람의 nickname이다.  
해당 channel이 반드시 존재하거나 유효해야 하는 요구 사항은 없다.  
초대 전용 채널(MODE + i)에 사용자를 초대하려면 초대를 보내는 client가 해당 channel operator로 인식되어야 한다.

---

#### 4.2.8. Kick command
```
   Command: KICK
Parameters: <channel> <user> [<comment>]

Numeric Replies:
  ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
  ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
  ERR_NOTONCHANNEL

Examples:
  KICK &Melbourne Matthew       ; "&Melbourne"에서 "Matthew"를 추방.

  KICK #Finnish John :Speaking English
                                ; #Finnish에서 "Speaking English"의 이유로 "John"을 추방.
```
KICK command는 user를 channel에서 강제로 제거하는데 사용된다.  
이는 사용자를 channel에서 "추방"한다 (강제 PART).  
  
Channel operator만 KICK이 가능하다.  
KICK 메시지를 받은 server는, 사용자를 추방하기 전에 발신자가 실제로 channel operator인지 확인한다.

---

### 4.4. Sending messages
IRC Protocol의 주요 목적은 client가 서로 통신할 수 있는 기반을 제공하는 것이다.  
PRIVMSG와 NOTICE는 실제로 한 client에서 다른 client로 텍스트 메시지를 전달하는 데 사용되는 유일한 메시지이다.  

---

#### 4.4.1. Private messages
```
   Command: PRIVMSG
Parameters: <receiver>{,<receiver>} <text to be sent>

Numeric Replies:
  ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
  ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
  ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
  ERR_NOSUCHNICK
  RPL_AWAY

Examples:
  PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br
                ; "Angel"에게 메시지 전송.
```
PRIVMSG는 사용자간 개인 메시지를 전송할 때 사용된다.  
`<receiver>`는 메시지를 받는 사용자의 nickname이다.  
`<receiver>`는 쉼표로 분리된 이름이나 채널의 목록일 수 있다.  
  
`<receiver>` 매개변수는 호스트 마스크 (#mask) 또는 서버 마스크 ($mask)일 수도 있다.  
두 경우 모두 server는 마스크와 일치하는 server 또는 host를 가진 사용자에게만 메시지를 보낸다.  
마스크에는 적어도 1개의 '.'이 있어야 하며, 마지막 '.' 뒤에 와일드카드('*', '?')가 없어야 한다.  
이러한 확장된 PRIVMSG는 operator(channel operator가 아닌, server 운영자)만 사용 가능하다.  

---

#### 4.4.2. Notice
```
   Command: NOTICE
Parameters: <nickname> <text>
```
NOTICE 메시지는 PRIVMSG와 비슷하다.  
NOTICE와 PRIVMSG와의 차이점은, NOTICE에 대한 자동 응답은 절대 전송되지 않아야 한다는 것이다.  
서버는 NOTICE 메시지에 대해 중계 역할만 해주고, 오류 응답과 같은 자동 응답을 생성하면 안된다.  
이는 자동 응답을 보내면서 루프에 빠지는 것을 피한다.

응답 및 예시에 대한 자세한 내용은 PRIVMSG를 참조.

---

### 4.6 Miscellaneous messages
이 섹션에서는 위에서 언급한 카테고리에는 속하지 않지만, 프로토콜에 필요한 메시지들을 설명한다.

---

#### 4.6.2. Ping message
```
   Command: PING
Parameters: <token>

Numeric Replies:
ERR_NOORIGIN                    ERR_NOSUCHSERVER

Example:
  PING WiZ               ; nickname "Wiz"에게 PING 메시지 전송.
```
PING 메시지는 현재 연결이 계속 연결되어 있는지 확인하기 위해 server, client에서 사용한다. 
다른 활동이 감지되지 않을 경우, 일정 간격으로 PING 메시지가 전송된다.  
일정 시간 내에 PING 메시지에 응답하지 않으면 해당 연결을 닫는다.  
  
PING 메시지를 받은 server 또는 client는 가능한 빨리 `<token>`(PING을 보낸 곳)에 적절한 PONG 메시지로 응답하여 여전히 활성 상태임을 나타내야 한다.

---

#### 4.6.3. Pong message
```
   Command: PONG
Parameters: [<server>] <token>

Numeric Replies:
ERR_NOORIGIN                    ERR_NOSUCHSERVER
```
PONG 메시지는 PING 메시지 응답에 사용된다.  
Server는 `<server>`매개변수를 꼭 사용해야하고, client는 해당 매개변수를 무시해야 한다.  
Client는 무조건 `<server>`매개변수를 사용하면 안된다.  
`<token>`은 PING을 받았을 때의 `<token>`과 동일해야 한다.

---

## 6. REPLIES
이 섹션에서는 지금까지 설명한 command들에 대한 숫자형 응답을 설명한다.  
각 숫자형 응답은 해당 숫자, 이름, 응답 문자열로 구성된다.

---

### 6.1 Error Replies
```
401     ERR_NOSUCHNICK
                "<nickname> :No such nick/channel"

        - Used to indicate the nickname parameter supplied to a
          command is currently unused.

402     ERR_NOSUCHSERVER
                "<server name> :No such server"

        - Used to indicate the server name given currently
          doesn't exist.

403     ERR_NOSUCHCHANNEL
                "<channel name> :No such channel"

        - Used to indicate the given channel name is invalid.

404     ERR_CANNOTSENDTOCHAN
                "<channel name> :Cannot send to channel"

        - Sent to a user who is either (a) not on a channel
          which is mode +n or (b) not a chanop (or mode +v) on
          a channel which has mode +m set and is trying to send
          a PRIVMSG message to that channel.

405     ERR_TOOMANYCHANNELS
                "<channel name> :You have joined too many channels"

        - Sent to a user when they have joined the maximum
          number of allowed channels and they try to join
          another channel.

406     ERR_WASNOSUCHNICK
                "<nickname> :There was no such nickname"

        - Returned by WHOWAS to indicate there is no history
          information for that nickname.

407     ERR_TOOMANYTARGETS
                "<target> :Duplicate recipients. No message \
                 delivered"

        - Returned to a client which is attempting to send a
          PRIVMSG/NOTICE using the user@host destination format
          and for a user@host which has several occurrences.

409     ERR_NOORIGIN
                ":No origin specified"

        - PING or PONG message missing the originator parameter
          which is required since these commands must work
          without valid prefixes.

411     ERR_NORECIPIENT
                ":No recipient given (<command>)"
412     ERR_NOTEXTTOSEND
                ":No text to send"
413     ERR_NOTOPLEVEL
                "<mask> :No toplevel domain specified"
414     ERR_WILDTOPLEVEL
                "<mask> :Wildcard in toplevel domain"

        - 412 - 414 are returned by PRIVMSG to indicate that
          the message wasn't delivered for some reason.
          ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that
          are returned when an invalid use of
          "PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.

421     ERR_UNKNOWNCOMMAND
                "<command> :Unknown command"

        - Returned to a registered client to indicate that the
          command sent is unknown by the server.

422     ERR_NOMOTD
                ":MOTD File is missing"

        - Server's MOTD file could not be opened by the server.

423     ERR_NOADMININFO
                "<server> :No administrative info available"

        - Returned by a server in response to an ADMIN message
          when there is an error in finding the appropriate
          information.

424     ERR_FILEERROR
                ":File error doing <file op> on <file>"

        - Generic error message used to report a failed file
          operation during the processing of a message.

431     ERR_NONICKNAMEGIVEN
                ":No nickname given"

        - Returned when a nickname parameter expected for a
          command and isn't found.

432     ERR_ERRONEUSNICKNAME
                "<nick> :Erroneus nickname"

        - Returned after receiving a NICK message which contains
          characters which do not fall in the defined set.  See
          section x.x.x for details on valid nicknames.

433     ERR_NICKNAMEINUSE
                "<nick> :Nickname is already in use"

        - Returned when a NICK message is processed that results
          in an attempt to change to a currently existing
          nickname.

436     ERR_NICKCOLLISION
                "<nick> :Nickname collision KILL"

        - Returned by a server to a client when it detects a
          nickname collision (registered of a NICK that
          already exists by another server).

441     ERR_USERNOTINCHANNEL
                "<nick> <channel> :They aren't on that channel"

        - Returned by the server to indicate that the target
          user of the command is not on the given channel.

442     ERR_NOTONCHANNEL
                "<channel> :You're not on that channel"

        - Returned by the server whenever a client tries to
          perform a channel effecting command for which the
          client isn't a member.

443     ERR_USERONCHANNEL
                "<user> <channel> :is already on channel"

        - Returned when a client tries to invite a user to a
          channel they are already on.

444     ERR_NOLOGIN
                "<user> :User not logged in"

        - Returned by the summon after a SUMMON command for a
          user was unable to be performed since they were not
          logged in.

445     ERR_SUMMONDISABLED
                ":SUMMON has been disabled"

        - Returned as a response to the SUMMON command.  Must be
          returned by any server which does not implement it.

446     ERR_USERSDISABLED
                ":USERS has been disabled"

        - Returned as a response to the USERS command.  Must be
          returned by any server which does not implement it.

451     ERR_NOTREGISTERED
                ":You have not registered"

        - Returned by the server to indicate that the client
          must be registered before the server will allow it
          to be parsed in detail.

461     ERR_NEEDMOREPARAMS
                "<command> :Not enough parameters"

        - Returned by the server by numerous commands to
          indicate to the client that it didn't supply enough
          parameters.

462     ERR_ALREADYREGISTRED
                ":You may not reregister"

        - Returned by the server to any link which tries to
          change part of the registered details (such as
          password or user details from second USER message).

463     ERR_NOPERMFORHOST
                ":Your host isn't among the privileged"

        - Returned to a client which attempts to register with
          a server which does not been setup to allow
          connections from the host the attempted connection
          is tried.

464     ERR_PASSWDMISMATCH
                ":Password incorrect"

        - Returned to indicate a failed attempt at registering
          a connection for which a password was required and
          was either not given or incorrect.

465     ERR_YOUREBANNEDCREEP
                ":You are banned from this server"

        - Returned after an attempt to connect and register
          yourself with a server which has been setup to
          explicitly deny connections to you.

467     ERR_KEYSET
                "<channel> :Channel key already set"
471     ERR_CHANNELISFULL
                "<channel> :Cannot join channel (+l)"
472     ERR_UNKNOWNMODE
                "<char> :is unknown mode char to me"
473     ERR_INVITEONLYCHAN
                "<channel> :Cannot join channel (+i)"
474     ERR_BANNEDFROMCHAN
                "<channel> :Cannot join channel (+b)"
475     ERR_BADCHANNELKEY
                "<channel> :Cannot join channel (+k)"
481     ERR_NOPRIVILEGES
                ":Permission Denied- You're not an IRC operator"

        - Any command requiring operator privileges to operate
          must return this error to indicate the attempt was
          unsuccessful.

482     ERR_CHANOPRIVSNEEDED
                "<channel> :You're not channel operator"

        - Any command requiring 'chanop' privileges (such as
          MODE messages) must return this error if the client
          making the attempt is not a chanop on the specified
          channel.

483     ERR_CANTKILLSERVER
                ":You cant kill a server!"

        - Any attempts to use the KILL command on a server
          are to be refused and this error returned directly
          to the client.

491     ERR_NOOPERHOST
                ":No O-lines for your host"

        - If a client sends an OPER message and the server has
          not been configured to allow connections from the
          client's host as an operator, this error must be
          returned.

501     ERR_UMODEUNKNOWNFLAG
                ":Unknown MODE flag"

        - Returned by the server to indicate that a MODE
          message was sent with a nickname parameter and that
          the a mode flag sent was not recognized.

502     ERR_USERSDONTMATCH
                ":Cant change mode for other users"

        - Error sent to any user trying to view or change the
          user mode for a user other than themselves.
```

---

### 6.2. Command responses
```
300     RPL_NONE
                Dummy reply number. Not used.

302     RPL_USERHOST
                ":[<reply>{<space><reply>}]"

        - Reply format used by USERHOST to list replies to
          the query list.  The reply string is composed as
          follows:

          <reply> ::= <nick>['*'] '=' <'+'|'-'><hostname>

          The '*' indicates whether the client has registered
          as an Operator.  The '-' or '+' characters represent
          whether the client has set an AWAY message or not
          respectively.

303     RPL_ISON
                ":[<nick> {<space><nick>}]"

        - Reply format used by ISON to list replies to the
          query list.

301     RPL_AWAY
                "<nick> :<away message>"
305     RPL_UNAWAY
                ":You are no longer marked as being away"
306     RPL_NOWAWAY
                ":You have been marked as being away"

        - These replies are used with the AWAY command (if
          allowed).  RPL_AWAY is sent to any client sending a
          PRIVMSG to a client which is away.  RPL_AWAY is only
          sent by the server to which the client is connected.
          Replies RPL_UNAWAY and RPL_NOWAWAY are sent when the
          client removes and sets an AWAY message.

311     RPL_WHOISUSER
                "<nick> <user> <host> * :<real name>"
312     RPL_WHOISSERVER
                "<nick> <server> :<server info>"
313     RPL_WHOISOPERATOR
                "<nick> :is an IRC operator"
317     RPL_WHOISIDLE
                "<nick> <integer> :seconds idle"
318     RPL_ENDOFWHOIS
                "<nick> :End of /WHOIS list"
319     RPL_WHOISCHANNELS
                "<nick> :{[@|+]<channel><space>}"

        - Replies 311 - 313, 317 - 319 are all replies
          generated in response to a WHOIS message.  Given that
          there are enough parameters present, the answering
          server must either formulate a reply out of the above
          numerics (if the query nick is found) or return an
          error reply.  The '*' in RPL_WHOISUSER is there as
          the literal character and not as a wild card.  For
          each reply set, only RPL_WHOISCHANNELS may appear
          more than once (for long lists of channel names).
          The '@' and '+' characters next to the channel name
          indicate whether a client is a channel operator or
          has been granted permission to speak on a moderated
          channel.  The RPL_ENDOFWHOIS reply is used to mark
          the end of processing a WHOIS message.

314     RPL_WHOWASUSER
                "<nick> <user> <host> * :<real name>"
369     RPL_ENDOFWHOWAS
                "<nick> :End of WHOWAS"

        - When replying to a WHOWAS message, a server must use
          the replies RPL_WHOWASUSER, RPL_WHOISSERVER or
          ERR_WASNOSUCHNICK for each nickname in the presented
          list.  At the end of all reply batches, there must
          be RPL_ENDOFWHOWAS (even if there was only one reply
          and it was an error).

321     RPL_LISTSTART
                "Channel :Users  Name"
322     RPL_LIST
                "<channel> <# visible> :<topic>"
323     RPL_LISTEND
                ":End of /LIST"

        - Replies RPL_LISTSTART, RPL_LIST, RPL_LISTEND mark
          the start, actual replies with data and end of the
          server's response to a LIST command.  If there are
          no channels available to return, only the start
          and end reply must be sent.

324     RPL_CHANNELMODEIS
                "<channel> <mode> <mode params>"

331     RPL_NOTOPIC
                "<channel> :No topic is set"
332     RPL_TOPIC
                "<channel> :<topic>"

        - When sending a TOPIC message to determine the
          channel topic, one of two replies is sent.  If
          the topic is set, RPL_TOPIC is sent back else
          RPL_NOTOPIC.

341     RPL_INVITING
                "<channel> <nick>"

        - Returned by the server to indicate that the
          attempted INVITE message was successful and is
          being passed onto the end client.

342     RPL_SUMMONING
                "<user> :Summoning user to IRC"

        - Returned by a server answering a SUMMON message to
          indicate that it is summoning that user.

351     RPL_VERSION
                "<version>.<debuglevel> <server> :<comments>"

        - Reply by the server showing its version details.
          The <version> is the version of the software being
          used (including any patchlevel revisions) and the
          <debuglevel> is used to indicate if the server is
          running in "debug mode".

          The "comments" field may contain any comments about
          the version or further version details.

352     RPL_WHOREPLY
                "<channel> <user> <host> <server> <nick> \
                 <H|G>[*][@|+] :<hopcount> <real name>"
315     RPL_ENDOFWHO
                "<name> :End of /WHO list"

        - The RPL_WHOREPLY and RPL_ENDOFWHO pair are used
          to answer a WHO message.  The RPL_WHOREPLY is only
          sent if there is an appropriate match to the WHO
          query.  If there is a list of parameters supplied
          with a WHO message, a RPL_ENDOFWHO must be sent
          after processing each list item with <name> being
          the item.

353     RPL_NAMREPLY
                "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
366     RPL_ENDOFNAMES
                "<channel> :End of /NAMES list"

        - To reply to a NAMES message, a reply pair consisting
          of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the
          server back to the client.  If there is no channel
          found as in the query, then only RPL_ENDOFNAMES is
          returned.  The exception to this is when a NAMES
          message is sent with no parameters and all visible
          channels and contents are sent back in a series of
          RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark
          the end.

364     RPL_LINKS
                "<mask> <server> :<hopcount> <server info>"
365     RPL_ENDOFLINKS
                "<mask> :End of /LINKS list"

        - In replying to the LINKS message, a server must send
          replies back using the RPL_LINKS numeric and mark the
          end of the list using an RPL_ENDOFLINKS reply.

367     RPL_BANLIST
                "<channel> <banid>"
368     RPL_ENDOFBANLIST
                "<channel> :End of channel ban list"

        - When listing the active 'bans' for a given channel,
          a server is required to send the list back using the
          RPL_BANLIST and RPL_ENDOFBANLIST messages.  A separate
          RPL_BANLIST is sent for each active banid.  After the
          banids have been listed (or if none present) a
          RPL_ENDOFBANLIST must be sent.

371     RPL_INFO
                ":<string>"
374     RPL_ENDOFINFO
                ":End of /INFO list"

        - A server responding to an INFO message is required to
          send all its 'info' in a series of RPL_INFO messages
          with a RPL_ENDOFINFO reply to indicate the end of the
          replies.

375     RPL_MOTDSTART
                ":- <server> Message of the day - "
372     RPL_MOTD
                ":- <text>"
376     RPL_ENDOFMOTD
                ":End of /MOTD command"

        - When responding to the MOTD message and the MOTD file
          is found, the file is displayed line by line, with
          each line no longer than 80 characters, using
          RPL_MOTD format replies.  These should be surrounded
          by a RPL_MOTDSTART (before the RPL_MOTDs) and an
          RPL_ENDOFMOTD (after).

381     RPL_YOUREOPER
                ":You are now an IRC operator"

        - RPL_YOUREOPER is sent back to a client which has
          just successfully issued an OPER message and gained
          operator status.

382     RPL_REHASHING
                "<config file> :Rehashing"

        - If the REHASH option is used and an operator sends
          a REHASH message, an RPL_REHASHING is sent back to
          the operator.

391     RPL_TIME
                "<server> :<string showing server's local time>"

        - When replying to the TIME message, a server must send
          the reply using the RPL_TIME format above.  The string
          showing the time need only contain the correct day and
          time there.  There is no further requirement for the
          time string.

392     RPL_USERSSTART
                ":UserID   Terminal  Host"
393     RPL_USERS
                ":%-8s %-9s %-8s"
394     RPL_ENDOFUSERS
                ":End of users"
395     RPL_NOUSERS
                ":Nobody logged in"

        - If the USERS message is handled by a server, the
          replies RPL_USERSTART, RPL_USERS, RPL_ENDOFUSERS and
          RPL_NOUSERS are used.  RPL_USERSSTART must be sent
          first, following by either a sequence of RPL_USERS
          or a single RPL_NOUSER.  Following this is
          RPL_ENDOFUSERS.

200     RPL_TRACELINK
                "Link <version & debug level> <destination> \
                 <next server>"
201     RPL_TRACECONNECTING
                "Try. <class> <server>"
202     RPL_TRACEHANDSHAKE
                "H.S. <class> <server>"
203     RPL_TRACEUNKNOWN
                "???? <class> [<client IP address in dot form>]"
204     RPL_TRACEOPERATOR
                "Oper <class> <nick>"
205     RPL_TRACEUSER
                "User <class> <nick>"
206     RPL_TRACESERVER
                "Serv <class> <int>S <int>C <server> \
                  <nick!user|*!*>@<host|server>"
208     RPL_TRACENEWTYPE
                "<newtype> 0 <client name>"
261     RPL_TRACELOG
                "File <logfile> <debug level>"

        - The RPL_TRACE* are all returned by the server in
          response to the TRACE message.  How many are
          returned is dependent on the the TRACE message and

211     RPL_STATSLINKINFO
                "<linkname> <sendq> <sent messages> \
                  <sent bytes> <received messages> \
                  <received bytes> <time open>"
212     RPL_STATSCOMMANDS
                "<command> <count>"
213     RPL_STATSCLINE
                "C <host> * <name> <port> <class>"
214     RPL_STATSNLINE
                "N <host> * <name> <port> <class>"
215     RPL_STATSILINE
                "I <host> * <host> <port> <class>"
216     RPL_STATSKLINE
                "K <host> * <username> <port> <class>"
218     RPL_STATSYLINE
                "Y <class> <ping frequency> <connect \
                  frequency> <max sendq>"
219     RPL_ENDOFSTATS
                "<stats letter> :End of /STATS report"
241     RPL_STATSLLINE
                "L <hostmask> * <servername> <maxdepth>"
242     RPL_STATSUPTIME
                ":Server Up %d days %d:%02d:%02d"
243     RPL_STATSOLINE
                "O <hostmask> * <name>"
244     RPL_STATSHLINE
                "H <hostmask> * <servername>"

221     RPL_UMODEIS
                "<user mode string>"

                - To answer a query about a client's own mode,
                  RPL_UMODEIS is sent back.

251     RPL_LUSERCLIENT
                ":There are <integer> users and <integer> \
                  invisible on <integer> servers"
252     RPL_LUSEROP
                "<integer> :operator(s) online"
253     RPL_LUSERUNKNOWN
                "<integer> :unknown connection(s)"
254     RPL_LUSERCHANNELS
                "<integer> :channels formed"
255     RPL_LUSERME
                ":I have <integer> clients and <integer> \
                  servers"

                - In processing an LUSERS message, the server
                  sends a set of replies from RPL_LUSERCLIENT,
                  RPL_LUSEROP, RPL_USERUNKNOWN,
                  RPL_LUSERCHANNELS and RPL_LUSERME.  When
                  replying, a server must send back
                  RPL_LUSERCLIENT and RPL_LUSERME.  The other
                  replies are only sent back if a non-zero count
                  is found for them.

256     RPL_ADMINME
                "<server> :Administrative info"
257     RPL_ADMINLOC1
                ":<admin info>"
258     RPL_ADMINLOC2
                ":<admin info>"
259     RPL_ADMINEMAIL
                ":<admin info>"

                - When replying to an ADMIN message, a server
                  is expected to use replies RLP_ADMINME
                  through to RPL_ADMINEMAIL and provide a text
                  message with each.  For RPL_ADMINLOC1 a
                  description of what city, state and country
                  the server is in is expected, followed by
                  details of the university and department
                  (RPL_ADMINLOC2) and finally the administrative
                  contact for the server (an email address here
                  is required) in RPL_ADMINEMAIL.
```

---

## 7. Client and server authentication
Client와 server는 동일한 수준의 인증을 받는다.  
두 경우 모두 server에 대한 모든 연결에 대해 IP 주소를 호스트 이름으로 조회하고, 해당 연결에 대해 비밀번호가 설정된 경우 비밀번호를 확인한다.  
이러한 확인은 모든 연결에서 가능하지만, 비밀번호 확인은 일반적으로 server에서만 사용된다.

---

