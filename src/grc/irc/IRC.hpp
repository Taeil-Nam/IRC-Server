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

#include "common.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"

using namespace gdf;

#define IRC_VERSION "v0.1"
#define CRLF "\r\n"

////////////////////////////////////////////////////////////
// Error Replies
////////////////////////////////////////////////////////////

// Indicates that no client can be found for the supplied nickname.
// The text used in the last param of this message may vary.
// "<client> <nickname> :No such nick/channel"
#define ERR_NOSUCHNICK "401"

// Used to indicate the given channel name is invalid.
// "<channel name> :No such channel"
#define ERR_NOSUCHCHANNEL "403"

// Returned by the PRIVMSG command to indicate the message
// wasn’t delivered because there was no recipient given.
// "<client> :No recipient given (<command>)"
#define ERR_NORECIPIENT "411"

// Returned by the PRIVMSG command to indicate the message
// wasn’t delivered because there was no text to send.
// "<client> :No text to send"
#define ERR_NOTEXTTOSEND "412"

// Indicates that the PRIVMSG / NOTICE could not be delivered to <channel>.
// The text used in the last param of this message may vary.
// This is generally sent in response to channel modes,
// such as a channel being moderated and the client
// not having permission to speak on the channel,
// or not being joined to a channel with the no external messages mode set.
// "<client> <channel> :Cannot send to channel"
#define ERR_CANNOTSENDTOCHAN "404"

// Indicates that the Message of the Day file does not exist
// or could not be found.
// The text used in the last param of this message may vary.
// "<client> :MOTD File is missing"
#define ERR_NOMOTD "422"

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

// Returned when a client tries to perform a channel+nick affecting command,
// when the nick isn’t joined to the channel (for example, MODE #channel +o nick).
// "<client> <nick> <channel> :They aren't on that channel"
#define ERR_USERNOTINCHANNEL "441"

// Returned by the server whenever a client tries to
// perform a channel effecting command for which the
// client isn't a member.
// "<client> <channel> :You're not on that channel"
#define ERR_NOTONCHANNEL  "442"

// Returned when a client tries to invite <nick> to a channel they’re already joined to.
// "<client> <nick> <channel> :is already on channel"
#define ERR_USERONCHANNEL "443"

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

// Indicates that a command failed because the client does not have
// the appropriate channel privileges. This numeric can apply for 
// different prefixes such as halfop, operator, etc. 
// The text used in the last param of this message may vary.
// "<client> <channel> :You're not channel operator"
#define ERR_CHANOPRIVSNEEDED "482"


////////////////////////////////////////////////////////////
// Command responses.
////////////////////////////////////////////////////////////

// The first message sent after client registration,
// this message introduces the client to the network.
// The text used in the last param of this message varies wildly.
// "<nick> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
#define RPL_WELCOME "001"

// Part of the post-registration greeting, this numeric returns
// the name and software/version of the server
// the client is currently connected to.
// The text used in the last param of this message varies wildly.
// "<client> :Your host is <servername>, running version <version>"
#define RPL_YOURHOST "002"

// Part of the post-registration greeting, this numeric returns
// a human-readable date/time that the server was started or created.
// The text used in the last param of this message varies wildly.
// "<client> :This server was created <datetime>"
#define RPL_CREATED "003"

// Part of the post-registration greeting.
// Clients SHOULD discover available features using RPL_ISUPPORT tokens
// rather than the mode letters listed in this reply.
// <client> <servername> <version> <available user modes> 
// <available channel modes> [<channel modes with a parameter>]
#define RPL_MYINFO "004"

// <client> <1-13 tokens> :are supported by this server
#define RPL_ISUPPORT "005"

// Sent to a client to inform them of the currently-set modes of a channel.
// <channel> is the name of the channel.
// <modestring> and <mode arguments> are a mode string
// and the mode arguments (delimited as separate parameters)
// as defined in the MODE message description.
// "<client> <channel> <modestring> <mode arguments>..."
#define RPL_CHANNELMODEIS "324"

// Sent to a client when joining a channel to inform them that
// the channel with the name <channel> does not have any topic set.
// "<client> <channel> :No topic is set"
#define RPL_NOTOPIC "331"

// Sent to a client when joining the <channel> to
// inform them of the current topic of the channel.
// When sending a TOPIC message to determine the
// channel topic, one of two replies is sent. If
// the topic is set, RPL_TOPIC is sent back else
// RPL_NOTOPIC.
// "<client> <channel> :<topic>"
#define RPL_TOPIC "332"

// Sent as a reply to the INVITE command to indicate that
// the attempt was successful and the client with
// the nickname <nick> has been invited to <channel>.
// "<client> <nick> <channel>"
#define RPL_INVITING "341"

// "<client> <symbol> <channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
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
// "<client> <channel> :End of /NAMES list"
#define RPL_ENDOFNAMES "366"

namespace grc
{

class IRC
{
public:
    static void HandleMessage(const int32 IN socket, Network& IN network, const std::string& IN password);
private:
    IRC(); // = delete
    typedef void (*TcommandFunctionPTR)(const int32,
                                        const std::string&,
                                        const std::vector<std::string>&,
                                        const std::string&,
                                        const std::string&,
                                        Network&);
    static void initializeCommandFunctionMap();
    static void parseMessage(const std::string& IN message,
                             std::string& OUT command,
                             std::vector<std::string>& OUT parameters,
                             std::string& OUT trailing);
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
    static void MODE(const int32 IN socket,
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void TOPIC(const int32 IN socket,
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void INVITE(const int32 IN socket,
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void KICK(const int32 IN socket,
                     const std::string& IN command,
                     const std::vector<std::string>& IN parameters,
                     const std::string& IN trailing,
                     const std::string& IN password,
                     Network& IN OUT network);
    static void PRIVMSG(const int32 IN socket,
                        const std::string& IN command,
                        const std::vector<std::string>& IN parameters,
                        const std::string& IN trailing,
                        const std::string& IN password,
                     Network& IN OUT network);
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

    static void sendWelcomeMessage(const int32 IN socket, Network& IN OUT network);
    static bool isNicknameInUse(const std::string& IN nickname);
private:
    static std::map<std::string, TcommandFunctionPTR> sStaticCommandFunctionMap;
};

}
