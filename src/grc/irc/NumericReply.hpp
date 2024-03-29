/**
 * @file NumericReply.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief NumericReply 상수 정의 파일
 * @version 0.1
 * @date 2024-03-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>

namespace grc
{

////////////////////////////////////////////////////////////
// Error Replies
////////////////////////////////////////////////////////////

// Used to indicate the given channel name is invalid.
// "<channel name> :No such channel"
const std::string ERR_NOSUCHCHANNEL = "403";

// Returned when a nickname parameter expected for a
// command and isn't found.
// ":No nickname given"
const std::string ERR_NONICKNAMEGIVEN = "431";

// Returned after receiving a NICK message which contains
// characters which do not fall in the defined set.  See
// section x.x.x for details on valid nicknames.
// "<nick> :Erroneus nickname"
const std::string ERR_ERRONEUSNICKNAME = "432";

// Returned when a NICK message is processed that results
// in an attempt to change to a currently existing
// nickname.
// "<nick> :Nickname is already in use"
const std::string ERR_NICKNAMEINUSE = "433";

// Returned by the server by numerous commands to
// indicate to the client that it didn't supply enough
// parameters.
// "<command> :Not enough parameters"
const std::string ERR_NEEDMOREPARAMS = "461";

// Returned by the server to any link which tries to
// change part of the registered details (such as
// password or user details from second USER message).
// ":You may not reregister"
const std::string ERR_ALREADYREGISTERED = "462";

// Returned to indicate a failed attempt at registering
// a connection for which a password was required and
// was either not given or incorrect.
// ":Password incorrect"
const std::string ERR_PASSWDMISMATCH = "464";

// "<channel> :Cannot join channel (+l)"
const std::string ERR_CHANNELISFULL = "471";

// "<channel> :Cannot join channel (+i)"
const std::string ERR_INVITEONLYCHAN = "473";

// "<channel> :Cannot join channel (+k)"
const std::string ERR_BADCHANNELKEY = "475";



////////////////////////////////////////////////////////////
// Command responses.
////////////////////////////////////////////////////////////

// The first message sent after client registration,
// this message introduces the client to the network.
// The text used in the last param of this message varies wildly.
// "<nick> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
const std::string RPL_WELCOME = "001";

// Sent to a client when joining the <channel> to
// inform them of the current topic of the channel.
// When sending a TOPIC message to determine the
// channel topic, one of two replies is sent. If
// the topic is set, RPL_TOPIC is sent back else
// RPL_NOTOPIC.
// "<channel> :<topic>"
const std::string RPL_TOPIC = "332";

}
