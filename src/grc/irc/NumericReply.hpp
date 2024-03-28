/**
 * @file NumericReply.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief NumericReply 정의 파일
 * @version 0.1
 * @date 2024-03-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

// Returned when a nickname parameter expected for a
// command and isn't found.
// ":No nickname given"
#define ERR_NONICKNAMEGIVEN 431

// Returned after receiving a NICK message which contains
// characters which do not fall in the defined set.  See
// section x.x.x for details on valid nicknames.
// "<nick> :Erroneus nickname"
#define ERR_ERRONEUSNICKNAME 432

// Returned when a NICK message is processed that results
// in an attempt to change to a currently existing
// nickname.
// "<nick> :Nickname is already in use"
#define ERR_NICKNAMEINUSE 433

// Returned by the server by numerous commands to
// indicate to the client that it didn't supply enough
// parameters.
// "<command> :Not enough parameters"
#define ERR_NEEDMOREPARAMS 461

// Returned by the server to any link which tries to
// change part of the registered details (such as
// password or user details from second USER message).
// ":You may not reregister"
#define ERR_ALREADYREGISTERED 462

// Returned to indicate a failed attempt at registering
// a connection for which a password was required and
// was either not given or incorrect.
// ":Password incorrect"
#define ERR_PASSWDMISMATCH 464
