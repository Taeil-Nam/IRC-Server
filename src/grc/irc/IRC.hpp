/**
 * @file IRC.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief 이 파일은 IRC 로직에 필요한 매크로 및 IRC 클래스를 정의한다.
 * @version 0.1
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "common.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"

using namespace gdf;

/**
 * @brief
 * IRC 서버의 버전을 나타내는 매크로.
 */
#define IRC_VERSION "v0.1"

/**
 * @brief Carriage Return Line Feed(CRLF)를 나타내는 매크로.
 */
#define CRLF "\r\n"

////////////////////////////////////////////////////////////
// Error Replies
////////////////////////////////////////////////////////////

/**
 * @brief 해당 nick을 가진 유저가 없는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> <nick> :No such nick/channel"
 */
#define ERR_NOSUCHNICK "401"

/**
 * @brief 해당 channel name을 가진 채널이 없는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<channel name> :No such channel"
 */
#define ERR_NOSUCHCHANNEL "403"

/**
 * @brief PRIVMSG에 받는 사람이 없는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> :No recipient given (<command>)"
 */
#define ERR_NORECIPIENT "411"

/**
 * @brief PRIVMSG에 전달할 메시지 내용이 없는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> :No text to send"
 */
#define ERR_NOTEXTTOSEND "412"

/**
 * @brief PRIVMSG / NOTICE 메시지가 주어진 채널에 전달될 수 없을 때 응답하는 에러 코드.
 * 유저가 채널에 메시지를 보낼 수 있는 권한이 없거나, 채널에 입장되어 있는 상태가 아닌 경우에 해당된다.
 *
 * 메시지 형식: "<client> <channel> :Cannot send to channel"
 */
#define ERR_CANNOTSENDTOCHAN "404"

/**
 * @brief 서버의 Message of the Day(MOTD) 파일이 존재하지 않는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> :MOTD File is missing"
 */
#define ERR_NOMOTD "422"

/**
 * @brief nick이 꼭 필요한 메시지에 nick이 없는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: ":No nick given"
 */
#define ERR_NONICKNAMEGIVEN "431"

/**
 * @brief NICK 메시지의 nick에 사용할 수 없는 문자가 들어있는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<nick> :Erroneus nick"
 */
#define ERR_ERRONEUSNICKNAME "432"

/**
 * @brief NICK 메시지의 nick을 이미 다른 유저가 사용하고 있는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<nick> :nick is already in use"
 */
#define ERR_NICKNAMEINUSE "433"

/**
 * @brief channel + nick의 조합을 사용하는 메시지에서, 해당 nick이 채널에 없는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> <nick> <channel> :They aren't on that channel"
 */
#define ERR_USERNOTINCHANNEL "441"

/**
 * @brief user가 속하지 않은 채널에 특정 메시지를 전달하려고 하는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> <channel> :You're not on that channel"
 */
#define ERR_NOTONCHANNEL "442"

/**
 * @brief 채널에 이미 있는 nick을 초대했을 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> <nick> <channel> :is already on channel"
 */
#define ERR_USERONCHANNEL "443"

/**
 * @brief 메시지에 필요한 매개변수가 부족한 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<command> :Not enough parameters"
 */
#define ERR_NEEDMOREPARAMS "461"

/**
 * @brief 연결 등록이 되지 않은 user로부터 메시지를 받은 경우 응답하는 에러 코드.
 *
 * 메시지 형식: ":You may not reregister"
 */
#define ERR_ALREADYREGISTERED "462"

/**
 * @brief 연결 등록을 위한 password가 다른 경우 응답하는 에러 코드.
 *
 * 메시지 형식: ":Password incorrect"
 */
#define ERR_PASSWDMISMATCH "464"

/**
 * @brief 유저가 가득찬 채널에 입장하려고 하는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<channel> :Cannot join channel (+l)"
 */
#define ERR_CHANNELISFULL "471"

/**
 * @brief 유저가 초대를 받지 않고 초대 전용 채널에 입장하는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<channel> :Cannot join channel (+i)"
 */
#define ERR_INVITEONLYCHAN "473"

/**
 * @brief 채널 입장에 필요한 key(password)가 없거나 다른 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<channel> :Cannot join channel (+k)"
 */
#define ERR_BADCHANNELKEY "475"

/**
 * @brief operator 권한이 없는 user가 operator 권한이 필요한 메시지를 사용하는 경우 응답하는 에러 코드.
 *
 * 메시지 형식: "<client> <channel> :You're not channel operator"
 */
#define ERR_CHANOPRIVSNEEDED "482"

////////////////////////////////////////////////////////////
// Command responses.
////////////////////////////////////////////////////////////

/**
 * @brief user가 등록되면 첫 번째로 응답하는 메시지이며, 해당 user의 정보를 알려준다.
 *
 * 메시지 형식: "<nick> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
 */
#define RPL_WELCOME "001"

/**
 * @brief user가 등록되면 두 번째로 응답하는 메시지이며, 해당 user가 연결된 서버의 정보를 알려준다.
 *
 * 메시지 형식: "<nick> :Your host is <servername>, running version <version>"
 */
#define RPL_YOURHOST "002"

/**
 * @brief user가 등록되면 세 번째로 응답하는 메시지이며, 해당 user가 연결된 서버가 생성된 날짜를 알려준다.
 *
 * 메시지 형식: "<nick> :This server was created <datetime>"
 */
#define RPL_CREATED "003"

/**
 * @brief user가 등록되면 네 번째로 응답하는 메시지이며, 서버 이름 및 version을 알려준다.
 *
 * 메시지 형식: "<nick> <servername> <version> <available user modes> ""
 */
#define RPL_MYINFO "004"

/**
 * @brief user가 등록되면 다섯 번째로 응답하는 메시지이며, 서버에서 지원 가능한 목록들을 알려준다.
 *
 * 메시지 형식: "<nick> <1-13 tokens> :are supported by this server"
 */
#define RPL_ISUPPORT "005"

/**
 * @brief 현재 채널에 어떤 mode가 적용되어 있는지 알려준다.
 *
 * 메시지 형식: "<nick> <channel> <modestring> <mode arguments>..."
 */
#define RPL_CHANNELMODEIS "324"

/**
 * @brief 유저가 채널에 입장했을 때, 채널에 topic이 없는 경우 topic이 설정되어 있지 않다고 알려준다.
 *
 * 메시지 형식: "<nick> <channel> :No topic is set"
 */
#define RPL_NOTOPIC "331"

/**
 * @brief 유저가 채널에 입장했을 때, 채널에 topic이 있는 경우 topic을 알려준다.
 *
 * 메시지 형식: "<nick> <channel> :<topic>"
 */
#define RPL_TOPIC "332"

/**
 * @brief 유저가 특정 유저를 채널에 초대했을 때, 정상적으로 초대했음을 알려준다.
 *
 * 메시지 형식: "<client> <target nick> <channel>"
 */
#define RPL_INVITING "341"

/**
 * @brief 채널의 모든 유저 목록을 알려준다.
 *
 * 메시지 형식: "<client> <symbol> <channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
 */
#define RPL_NAMREPLY "353"

/**
 * @brief 채널의 모든 유저 목록의 끝을 알려준다.
 * RPL_NAMREPLY 메시지의 뒤에 따라온다.
 *
 * 메시지 형식: "<client> <channel> :End of /NAMES list"
 */
#define RPL_ENDOFNAMES "366"

namespace grc
{

    /**
     * @class IRC
     * @brief IRC 로직을 수행하는 정적 클래스.
     *
     */
    class IRC
    {
    public:
        /**
         * @brief 유저로부터 들어온 메시지를 처리하는 함수.
         *
         * 네트워크를 통해 수신된 메시지가 있는 경우, Core::Run()에서 호출된다.\n
         * CRLF로 끝나는 메시지를 하나씩 가져와서 command, parameters, trailing 으로 파싱한다.\n
         * command에 알맞는 함수를 호출한다.\n
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         * @param password 유저 연결 등록에 필요한 비밀번호.
         */
        static void HandleMessage(const int32 IN socket, Network &IN network, const std::string &IN password);

    private:
        /**
         * @brief IRC 객체의 기본 생성자. (사용되지 않음)
         * 
         */
        IRC(); // = delete
        /**
         * @brief command 함수 포인터를 정의.
         *
         */
        typedef void (*TcommandFunctionPTR)(const int32,
                                            const std::string &,
                                            const std::vector<std::string> &,
                                            const std::string &,
                                            const std::string &,
                                            Network &);
        /**
         * @brief 정적 멤버 변수 sStaticCommandFunctionMap를 초기화하는 함수.
         *
         */
        static void initializeCommandFunctionMap();
        /**
         * @brief CRLF로 끝나는 메시지를 파싱하는 함수.
         *
         * 메시지 파싱 후 매개 변수로 받은 command, parameters, trailing에 올바른 값들을 넣어준다.
         *
         * [Input message format]\n
         * <command> <parameters> <trailing>\n
         * <trailing> = ':'로 시작하는 맨 마지막 문자열.
         *
         * @param message 원본 메시지.
         * @param command command가 저장되는 std::string.
         * @param parameters parameter들이 저장되는 std::vector<std::string>.
         * @param trailing trailing이 저장되는 std::string.
         */
        static void parseMessage(const std::string &IN message,
                                 std::string &OUT command,
                                 std::vector<std::string> &OUT parameters,
                                 std::string &OUT trailing);
        /**
         * @brief PASS 메시지를 처리하는 함수.
         *
         * 유저를 인증한다.
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_PASSWDMISMATCH : password가 다른 경우.\n
         * - ERR_ALREADYREGISTERED : 유저가 이미 등록되어 있는 경우.\n
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 유저 연결 등록에 필요한 비밀번호.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void PASS(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief NICK 메시지를 처리하는 함수.
         *
         * 유저의 nickname을 설정한다.
         *
         * nickname이 올바르게 설정된 경우, 해당 유저가 속한 모든 채널에 알린다.
         *
         * [Error Replies]\n
         * - ERR_NONICKNAMEGIVEN : 메시지에 nickname이 없는 경우.\n
         * - ERR_ERRONEUSNICKNAME : 사용할 수 없는 문자가 nickname에 포함되어 있는 경우.\n
         * - ERR_NICKNAMEINUSE : 이미 해당 nickname을 사용하는 유저가 있는 경우.\n
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void NICK(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief USER 메시지를 처리하는 함수.
         *
         * 유저의 username, hostname, servername, realname을 설정한다.
         *
         * 유저 설정을 마친 후, 유저가 연결되었는지 확인을 위해 PING 메시지를 전송한다.\n
         * 유저가 PING 메시지에 대한 PONG 메시지를 응답한 경우, 해당 유저의 연결을 등록한다.
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_ALREADYREGISTERED : 이미 유저가 등록된 경우.\n
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void USER(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief QUIT 메시지를 처리하는 함수.
         *
         * 유저를 종료시킨다.
         *
         * 유저가 특정 채널에 존재하는 경우, 해당 채널에 유저가 종료되었음을 알린다.
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void QUIT(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief JOIN 메시지를 처리하는 함수.
         *
         * 유저를 특정 채널에 입장시킨다.
         *
         * 유저가 새로운 채널에 입장하는 경우, 채널의 operator로 설정된다.\n
         * 유저가 기존 채널에 입장하는 경우, 채널의 일반 유저로 설정된다.\n
         * 채널의 모든 유저에게 새로운 유저의 입장을 알린다.
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_INVITEONLYCHAN : 채널이 초대 전용으로 설정되어 있으나, 초대 받지 않은 상태인 경우.\n
         * - ERR_BADCHANNELKEY : 채널에 key가 설정되어 있으나, 올바르지 않은 key로 입장한 경우.\n
         * - ERR_CHANNELISULL : 채널이 꽉 찬 경우.
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void JOIN(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief PART 메시지를 처리하는 함수.
         *
         * 유저를 채널에서 퇴장시킨다.
         *
         * 채널의 모든 유저에게 유저의 퇴장을 알린다.
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_NOSUCHCHANNEL : 없는 채널인 경우.\n
         * - ERR_NOTONCHANNEL : 유저가 채널에 없는 경우.\n
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void PART(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief MODE 메시지를 처리하는 함수.
         *
         * 채널에 대한 옵션을 설정한다.
         * 
         * 옵션 설정 후, 채널의 모든 유저에게 알린다.
         *
         * [Options]\n
         * - o : 특정 유저에게 채널의 operator 권한을 부여하거나 해제한다.\n
         * - t : 채널의 topic 변경을 operator만 가능하도록 설정하거나 해제한다.\n
         * - i : 채널을 초대 전용으로 설정하거나 해제한다.\n
         * - l : 채널의 최대 유저 수를 설정하거나 해제한다.\n
         * - k : 채널 입장에 필요한 key(password)를 설정하거나 해제한다.
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_NOSUCHCHANNEL : 없는 채널인 경우.\n
         * - ERR_CHANOPRIVSNEEDED : 메시지를 보낸 유저가 operator가 아닌 경우.\n
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void MODE(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief TOPIC 메시지를 처리하는 함수.
         *
         * 채널의 topic을 설정하거나 삭제한다.\n
         * trailing이 없는 경우, 채널의 topic을 알려준다.
         * 
         * topic 설정 및 삭제 후, 채널의 모든 유저에게 알린다.
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_NOSUCHCHANNEL : 없는 채널인 경우.\n
         * - ERR_NOTONCHANNEL : 유저가 채널에 없는 경우.\n
         * - ERR_CHANOPRIVSNEEDED : 메시지를 보낸 유저가 operator가 아닌 경우.
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void TOPIC(const int32 IN socket,
                          const std::string &IN command,
                          const std::vector<std::string> &IN parameters,
                          const std::string &IN trailing,
                          const std::string &IN password,
                          Network &IN OUT network);
        /**
         * @brief INVITE 메시지를 처리하는 함수.
         *
         * 특정 유저를 채널에 초대한다.\n
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_NOSUCHCHANNEL : 없는 채널인 경우.\n
         * - ERR_NOTONCHANNEL : 유저가 채널에 없는 경우.\n
         * - ERR_CHANOPRIVSNEEDED : 메시지를 보낸 유저가 operator가 아닌 경우.\n
         * - ERR_USERONCHANNEL : 유저가 이미 채널에 존재하는 경우.
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void INVITE(const int32 IN socket,
                           const std::string &IN command,
                           const std::vector<std::string> &IN parameters,
                           const std::string &IN trailing,
                           const std::string &IN password,
                           Network &IN OUT network);
        /**
         * @brief KICK 메시지를 처리하는 함수.
         *
         * 특정 유저를 채널에서 강제로 추방시킨다.
         * 
         * 유저 추방 후, 채널의 모든 유저에게 알린다.
         *
         * [Error Replies]\n
         * - ERR_NEEDMOREPARAMS : parameter가 부족한 경우.\n
         * - ERR_NOSUCHCHANNEL : 없는 채널인 경우.\n
         * - ERR_NOTONCHANNEL : 유저가 채널에 없는 경우.\n
         * - ERR_CHANOPRIVSNEEDED : 메시지를 보낸 유저가 operator가 아닌 경우.\n
         * - ERR_USERNOTINCHANNEL : 추방시킬 유저가 채널에 존재하지 않는 경우.
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void KICK(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief PRIVMSG 메시지를 처리하는 함수.
         *
         * 유저 또는 채널에 메시지를 전송한다.
         *
         * [Error Replies]\n
         * - ERR_NORECIPIENT : 메시지를 전달할 목적지가 포함되어 있지 않은 경우.\n
         * - ERR_NOTEXTTOSEND : 전달할 메시지가 포함되어 있지 않은 경우.\n
         * - ERR_CANNOTSENDTOCHAN : 메시지를 보내는 유저가 해당 채널에 없는 경우.\n
         * - ERR_NOSUCHNICK : 메시지를 전송할 유저나 채널 이름이 존재하지 않는 경우.\n
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void PRIVMSG(const int32 IN socket,
                            const std::string &IN command,
                            const std::vector<std::string> &IN parameters,
                            const std::string &IN trailing,
                            const std::string &IN password,
                            Network &IN OUT network);
        /**
         * @brief PING 메시지를 처리하는 함수.
         *
         * PING 메시지를 보낸 유저에게 PONG 메시지를 전송한다.
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void PING(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);
        /**
         * @brief PONG 메시지를 처리하는 함수.
         *
         * 유저가 PONG 메시지를 보낸 경우, 해당 유저를 등록시킨다.
         *
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param command command.
         * @param parameters parameters.
         * @param trailing trailing.
         * @param password 사용되지 않음.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void PONG(const int32 IN socket,
                         const std::string &IN command,
                         const std::vector<std::string> &IN parameters,
                         const std::string &IN trailing,
                         const std::string &IN password,
                         Network &IN OUT network);

        /**
         * @brief 웰컴 메시지를 전송하는 함수.
         * 
         * 유저가 등록되면, 웰컴 메시지를 전송한다.
         * 
         * @param socket 메시지를 보낸 유저의 소켓.
         * @param network 메시지 처리에 사용될 네트워크 객체.
         */
        static void sendWelcomeMessage(const int32 IN socket, Network &IN OUT network);
        /**
         * @brief nickname 중복을 확인하는 함수.
         * 
         * @param nickname 찾을 nickname.
         * @return true nickname이 이미 사용 중인 경우, true 반환.
         * @return false nickname이 사용 중이지 않은 경우, false 반환.
         */
        static bool isNicknameInUse(const std::string &IN nickname);

    private:
        /**
         * @brief IRC command에 따라 호출해야 하는 함수를 가지고 있는 정적 멤버 변수.
         * 
         * Key = IRC command, Value = command의 함수.
         */
        static std::map<std::string, TcommandFunctionPTR> sStaticCommandFunctionMap;
    };

}
