/**
 * @file IRC.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief IRC 로직을 처리하는 정적 클래스
 * @version 0.1
 * @date 2024-03-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "BSD-GDF/Network/Network.hpp"
#include "common.hpp"
#include "../core/Core.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"

using namespace gdf;

#define CRLF "\r\n"

////////////////////////////////////////////////////////////
// Error Replies
////////////////////////////////////////////////////////////

// Used to indicate the given channel name is invalid.
// "<channel name> :No such channel"
#define ERR_NOSUCHCHANNEL "403"

// Returned when a nickname parameter expected for a
// command and isn't found.
// ":No nickname given"
#define ERR_NONICKNAMEGIVEN "431"

// Returned after receiving a NICK message which contains
// characters which do not fall in the defined set.  See
// section x.x.x for details on valid nicknames.
// "<nick> :Erroneus nickname"
#define ERR_ERRONEUSNICKNAME "432"

// Returned when a NICK message is processed that results
// in an attempt to change to a currently existing
// nickname.
// "<nick> :Nickname is already in use"
#define ERR_NICKNAMEINUSE "433"

// Returned by the server whenever a client tries to
// perform a channel effecting command for which the
// client isn't a member.
// "<client> <channel> :You're not on that channel"
#define ERR_NOTONCHANNEL  "442"


// Returned by the server by numerous commands to
// indicate to the client that it didn't supply enough
// parameters.
// "<command> :Not enough parameters"
#define ERR_NEEDMOREPARAMS "461"

// Returned by the server to any link which tries to
// change part of the registered details (such as
// password or user details from second USER message).
// ":You may not reregister"
#define ERR_ALREADYREGISTERED "462"

// Returned to indicate a failed attempt at registering
// a connection for which a password was required and
// was either not given or incorrect.
// ":Password incorrect"
#define ERR_PASSWDMISMATCH "464"

// "<channel> :Cannot join channel (+l)"
#define ERR_CHANNELISFULL "471"

// "<channel> :Cannot join channel (+i)"
#define ERR_INVITEONLYCHAN "473"

// "<channel> :Cannot join channel (+k)"
#define ERR_BADCHANNELKEY "475"



////////////////////////////////////////////////////////////
// Command responses.
////////////////////////////////////////////////////////////

// The first message sent after client registration,
// this message introduces the client to the network.
// The text used in the last param of this message varies wildly.
// "<nick> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
#define RPL_WELCOME "001"

// Sent to a client when joining the <channel> to
// inform them of the current topic of the channel.
// When sending a TOPIC message to determine the
// channel topic, one of two replies is sent. If
// the topic is set, RPL_TOPIC is sent back else
// RPL_NOTOPIC.
// "<client nick> <channel> :<topic>"
#define RPL_TOPIC "332"

// "<client nick> <symbol> <channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
#define RPL_NAMREPLY "353"

// To reply to a NAMES message, a reply pair consisting
// of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the
// server back to the client.  If there is no channel
// found as in the query, then only RPL_ENDOFNAMES is
// returned.  The exception to this is when a NAMES
// message is sent with no parameters and all visible
// channels and contents are sent back in a series of
// RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark
// the end.
// "<client nick> <channel> :End of /NAMES list"
#define RPL_ENDOFNAMES "366"

namespace grc
{

class IRC
{
public:
    enum eIRCCommand
    {
        kPass = 0,
        kNick,
        kUser,
        kQuit,
        kJoin,
        kPart,
        kMode,
        kTopic,
        kInvite,
        kKick,
        kPrivmsg,
        kPing,
        kPong,
        kIRCCommandSize
    };

public:
    static void CheckUserConnection(const int32 IN socket, Network& IN network);
    static void HandleMessage(const int32 IN socket, Network& IN network, const std::string& IN password);
private:
    IRC(); // = delete

    static void parseMessage(const std::string& IN message,
                             std::string& OUT command,
                             std::vector<std::string>& OUT parameters,
                             std::string OUT trailing);
    static void PASS(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void NICK(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void USER(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void QUIT(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void JOIN(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void PART(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    // static void MODE(const int32 IN socket, 
    //                  const std::string& IN command,
    //                  const std::vector<std::string>& IN parameters,
    //                  const std::string& IN trailing,
    //                  const std::string& IN password,
    //                  Network& IN OUT network);
    // static void TOPIC(const int32 IN socket, 
    //                  const std::string& IN command,
    //                  const std::vector<std::string>& IN parameters,
    //                  const std::string& IN trailing,
    //                  const std::string& IN password,
    //                  Network& IN OUT network);
    // static void INVITE(const int32 IN socket, 
    //                  const std::string& IN command,
    //                  const std::vector<std::string>& IN parameters,
    //                  const std::string& IN trailing,
    //                  const std::string& IN password,
    //                  Network& IN OUT network);
    // static void KICK(const int32 IN socket, 
    //                  const std::string& IN command,
    //                  const std::vector<std::string>& IN parameters,
    //                  const std::string& IN trailing,
    //                  const std::string& IN password,
    //                  Network& IN OUT network);
    // static void PRIVMSG(const int32 IN socket, 
    //                  const std::string& IN command,
    //                  const std::vector<std::string>& IN parameters,
    //                  const std::string& IN trailing,
    //                  const std::string& IN password,
    //                  Network& IN OUT network);
    static void PING(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void PONG(const int32 IN socket, 
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);

    static bool isNicknameInUse(const std::string& IN nickname);

private:
    const static std::string mCommands[kIRCCommandSize];
};

}
