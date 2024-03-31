/**
 * @file IRCMessage.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief IRC 메시지를 처리하는 정적 클래스
 * @version 0.1
 * @date 2024-03-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "common.hpp"

namespace grc
{

class IRCMessage
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
	static void PASS(const int32 IN socket, const std::string& IN message);
    // static void NICK(const int32 IN socket,
    //     const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // static void USER(const int32 IN socket,
    //     const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // static void QUIT(const int32 IN socket,
    //     const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // static void JOIN(const int32 IN socket, const std::vector<std::string>& IN leading);
    // static void PART(const int32 IN socket, const std::vector<std::string>& IN leading);
    // // static void MODE(const int32 IN socket,
    //     // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // // static void TOPIC(const int32 IN socket,
    //     // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // // static void INVITE(const int32 IN socket,
    //     // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // // static void KICK(const int32 IN socket,
    //     // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // // static void PRIVMSG(const int32 IN socket,
    //     // const std::vector<std::string>& IN leading, const std::string& IN trailing);
    // static void PING(const int32 IN socket, const std::vector<std::string>& IN leading);
    // static void PONG(const int32 IN socket, const std::vector<std::string>& IN leading);

private:
	IRCMessage(); // = delete

	/**
	 * @brief message 매개 변수를 파싱 후, command, parameters, trailing에 알맞은 값 저장.
	 * 
	 * @param message[IN]: Message to parse.
	 * @param command[OUT]: Command of message.
	 * @param parameters[OUT]: Parameters of command.
	 * @param trailing[OUT]: Last string of message that start with ':'.
	 */
	static void parseMessage
	(
		const std::string& IN message,
		std::string& OUT command,
		std::vector<std::string>& OUT parameters,
		std::string OUT trailing
	);

private:
	const static std::string mCommands[kIRCCommandSize];
};

}
