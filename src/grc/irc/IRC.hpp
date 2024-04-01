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
#include "NumericReply.hpp"
#include "Channel.hpp"
#include "User.hpp"

using namespace gdf;

#define CRLF "\r\n"

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
    static void AddUser(const int32 IN socket, const User& IN user);
    static void DeleteUser(const int32 IN socket);
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
    static std::map<int32, User> mUsers;
    static std::map<std::string, Channel> mChannels;
};

}
