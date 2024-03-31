#include "IRCMessage.hpp"

namespace grc
{

const std::string IRCMessage::mCommands[kIRCCommandSize]
=
{
	"PASS",
	"NICK",
	"USER",
	"QUIT",
	"JOIN",
	"PART",
	"MODE",
	"TOPIC",
	"INVITE",
	"KICK",
	"PRIVMSG",
	"PING",
	"PONG"
};

void IRCMessage::PASS(const int32 IN socket, const std::string& IN message)
{
	std::string command("");
	std::vector<std::string> parameters;
	std::string trailing("");
	parseMessage(message, command, parameters, trailing);
	// TODO(tnam) : PASS 로직 추가
}

void IRCMessage::parseMessage
(
	const std::string& IN message,
	std::string& OUT command,
	std::vector<std::string>& OUT parameters,
	std::string OUT trailing
)
{
	// [Input message format]
	// command | parameters | [trailing]
	// TODO(tnam) : message 파싱 및 OUT 매개변수들에 알맞은 값 저장
}

// void Core::processPASSMessage(const int32 IN socket,
//     const std::vector<std::string>& IN leading, const std::string& IN trailing)
// {
//     const size_t parameterSize = leading.size() - 1 + (trailing == "" ? 0 : 1);
//     const std::string& command = leading[0];
//     // Client didn't authenticated
//     if (command != "PASS")
//     {
//         LOG(LogLevel::Warning) << "Client(IP: " << mNetwork.GetIPString(socket)
//             << ") didn't authenticated" << "(PASS)";
//         mNetwork.ClearRecvBuffer(socket);
//         mNetwork.DisconnectClient(socket);
//         return;
//     }
//     // ERR_NEEDMOREPARAMS
//     if (parameterSize  < 1)
//     {
//         LOG(LogLevel::Warning) << "Client(IP: " << mNetwork.GetIPString(socket)
//             << ") hasn't Password" << "(PASS)";
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_NEEDMOREPARAMS + " " + command + " :Not enough parameters" + "\r\n");
//         mNetwork.SendToClient(socket);
//         mNetwork.DisconnectClient(socket);
//         return;
//     }
//     const std::string& password = leading[1];
//     User& user = mUsers[socket];
//     // ERR_ALREADYREGISTERED
//     if (user.IsAuthenticated())
//     {
//         mNetwork.FetchToSendBuffer(socket, 
//             ERR_ALREADYREGISTERED + " :You may not reregister" + "\r\n");
//         return;
//     }
//     // ERR_PASSWDMISMATCH
//     if (mPassword != password)
//     {
//         LOG(LogLevel::Warning) << "Client(IP: " << mNetwork.GetIPString(socket)
//             << ") used incorrect password" << "(PASS)";
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_PASSWDMISMATCH + " :Password incorrect" + "\r\n");
//         mNetwork.SendToClient(socket);
//         mNetwork.DisconnectClient(socket);
//         return;
//     }
//     user.SetAuthenticated();
// }

// void Core::processNICKMessage(const int32 IN socket,
//     const std::vector<std::string>& IN leading, const std::string& IN trailing)
// {
//     const size_t parameterSize = leading.size() - 1 + (trailing == "" ? 0 : 1);
//     // ERR_NONICKNAMEGIVEN
//     if (parameterSize < 1)
//     {
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_NONICKNAMEGIVEN + " :No nickname given" + "\r\n");
//         return;
//     }
//     std::string nickname;
//     for (size_t i = 1; i < leading.size(); i++)
//     {
//         nickname += leading[i];
//         if (i < leading.size() - 1)
//         {
//             nickname += " ";
//         }
//     }
//     // ERR_ERRONEUSNICKNAME
//     if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '&' || nickname[0] == '#'
//         || nickname.find(' ') != std::string::npos || nickname.find(',') != std::string::npos
//         || nickname.find('*') != std::string::npos || nickname.find('?') != std::string::npos
//         || nickname.find('!') != std::string::npos || nickname.find('@') != std::string::npos
//         || nickname.find('.') != std::string::npos || nickname.size() > 9)
//     {
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_ERRONEUSNICKNAME + " " + nickname + " :Erroneus nickname" + "\r\n");
//         return;
//     }
//     // ERR_NICKNAMEINUSE
//     if (isNicknameInUse(nickname))
//     {
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_NICKNAMEINUSE + " " + nickname + " :" + nickname + "\r\n");
//         return;
//     }
//     User& user = mUsers[socket];
//     user.SetNickname(nickname);
// }

// void Core::processUSERMessage(const int32 IN socket,
//     const std::vector<std::string>& IN leading, const std::string& IN trailing)
// {
//     const size_t parameterSize = leading.size() - 1 + (trailing == "" ? 0 : 1);
//     const std::string& command = leading[0];
//     // ERR_NEEDMOREPARAMS
//     if (parameterSize < 4)
//     {
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_NEEDMOREPARAMS + " " + command + " :Not enough parameters" + "\r\n");
//         return;
//     }
//     User& user = mUsers[socket];
//     // ERR_ALREADYREGISTERED
//     if (user.IsRegistered())
//     {
//         mNetwork.FetchToSendBuffer(socket, 
//             ERR_ALREADYREGISTERED + " :You may not reregister" + "\r\n");
//         return;
//     }
//     const std::string& username = leading[1];
//     const std::string& hostname = leading[2];
//     const std::string& servername = leading[3];
//     const std::string& realname = trailing;
//     user.SetUsername(username);
//     user.SetHostname(hostname);
//     user.SetServername(servername);
//     user.SetRealname(realname);
//     user.SetRegistered();
//     mNetwork.FetchToSendBuffer(socket, 
//             RPL_WELCOME + " " + user.GetNickname()
//             + " :Welcome to the GameRelayChat Network, " + user.GetNickname() + "!"
//             + user.GetUsername() + "@" + user.GetHostname() + "\r\n");
//     LOG(LogLevel::Informational) << "User [" << user.GetNickname() << "] Registered";
// }

// void Core::processJOINMessage(const int32 IN socket, const std::vector<std::string>& IN leading)
// {
//     ///// Ex) JOIN #foo,#bar fubar,foobar /////
//     const size_t parameterSize = leading.size() - 1;
//     const std::string& command = leading[0];
//     // ERR_NEEDMOREPARAMS
//     if (parameterSize < 1)
//     {
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_NEEDMOREPARAMS + " " + command + " :Not enough parameters" + "\r\n");
//         return;
//     }
//     const std::vector<std::string> channels = split(leading[1], ","); // #foo, #bar
//     std::vector<std::string> keys;
//     if (parameterSize >= 2)
//     {
//         keys = split(leading[2], ","); // fubar, foobar
//     }

//     const User& user = mUsers[socket];
//     for (std::size_t i = 0; i < channels.size(); i++)
//     {
//         const std::string& channelName = channels[i];
//         // ERR_NOSUCHCHANNEL
//         if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
//             || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos)
//         {
//             mNetwork.FetchToSendBuffer(socket,
//                 ERR_NOSUCHCHANNEL + " " + channelName + " :No such channel" + "\r\n");
//             return;
//         }
//         std::string key;
//         if (i < keys.size())
//         {
//             key = keys[i];
//         }
//         if (mChannels.count(channelName) == 0)
//         {
//             mChannels[channelName].SetName(channelName);
//             mChannels[channelName].AddUser(user.GetNickname(), user);
//             mChannels[channelName].AddOperator(user.GetNickname(), user);
//             // TODO : hello bot(client)를 해당 채널에 추가(bonus)
//             LOG(LogLevel::Informational) << "Create new channel " << "[" << channelName << "]";
//             LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
//                 << " join to channel " << "[" << channelName << "]";
//             LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
//                 << " is now operator of channel " << "[" << channelName << "]";
//         }
//         else
//         {
//             // ERR_INVITEONLYCHAN
//             if (mChannels[channelName].IsInviteOnly())
//             {
//                 mNetwork.FetchToSendBuffer(socket,
//                 ERR_INVITEONLYCHAN + " " + channelName + " :Cannot join channel (+i)" + "\r\n");
//                 return;
//             }
//             // ERR_BADCHANNELKEY
//             if (mChannels[channelName].IsKeyRequired())
//             {
//                 if (key != mChannels[channelName].GetKey())
//                 {
//                     mNetwork.FetchToSendBuffer(socket,
//                     ERR_BADCHANNELKEY + " " + channelName + " :Cannot join channel (+k)" + "\r\n");
//                     return;
//                 }
//             }
//             // ERR_CHANNELISFULL
//             if (mChannels[channelName].GetMaxUserCount() != Channel::UNLIMIT
//                 && mChannels[channelName].GetMaxUserCount() <= mChannels[channelName].GetCurrentUserCount())
//             {
//                 mNetwork.FetchToSendBuffer(socket,
//                     ERR_CHANNELISFULL + " " + channelName + " :Cannot join channel (+l)" + "\r\n");
//                     return;
//             }
//             mChannels[channelName].AddUser(user.GetNickname(), user);
//         }
//         std::map<std::string, User>::const_iterator channelUser = mChannels[channelName].GetUsers().begin();
//         while (channelUser != mChannels[channelName].GetUsers().end())
//         {
//             int32 socket = channelUser->second.GetSocket();
//             mNetwork.FetchToSendBuffer(socket,
//                 ":" + user.GetNickname() + " " + "JOIN" + " " + channelName + "\r\n");
//             mNetwork.FetchToSendBuffer(socket,
//                 RPL_TOPIC + " " + user.GetNickname() + " " + channelName
//                 + " :" + mChannels[channelName].GetTopic() + "\r\n");
//             mNetwork.FetchToSendBuffer(socket,
//                 RPL_NAMREPLY + " " + user.GetNickname() + " " + "=" + " " + channelName + " :"
//                 + mChannels[channelName].GetAllUsersNickname() + "\r\n");
//             mNetwork.FetchToSendBuffer(socket,
//                 RPL_ENDOFNAMES + " " + user.GetNickname() + " " + channelName + " :End of /NAMES list" + "\r\n");
//             channelUser++;
//         }
//     }
// }

// void Core::processPARTMessage(const int32 IN socket, const std::vector<std::string>& IN leading)
// {
//     const size_t parameterSize = leading.size() - 1;
//     const std::string& command = leading[0];
//     // ERR_NEEDMOREPARAMS
//     if (parameterSize < 1)
//     {
//         mNetwork.FetchToSendBuffer(socket,
//             ERR_NEEDMOREPARAMS + " " + command + " :Not enough parameters" + "\r\n");
//         return;
//     }
//     const std::vector<std::string> channels = split(leading[1], ",");
//     for (size_t i = 0; i < channels.size(); i++)
//     {
//         const std::string& channelName = channels[i];
//         // ERR_NOSUCHCHANNEL
//         if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
//             || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos
//             || mChannels.count(channelName) == 0)
//         {
//             mNetwork.FetchToSendBuffer(socket,
//                 ERR_NOSUCHCHANNEL + " " + channelName + " :No such channel" + "\r\n");
//             continue;
//         }
//         // ERR_NOTONCHANNEL
//         User& user = mUsers[socket];
//         if (mChannels[channelName].IsUserExist(user.GetNickname()) == false)
//         {
//             mNetwork.FetchToSendBuffer(socket,
//                 ERR_NOTONCHANNEL + " " + user.GetNickname() + " "
//                 + channelName + " :You're not on that channel" + "\r\n");
//             continue;
//         }
//         std::map<std::string, User>::const_iterator channelUser = mChannels[channelName].GetUsers().begin();
//         while (channelUser != mChannels[channelName].GetUsers().end())
//         {
//             // TODO: PART가 아닌, PRIVMSG로 응답해줘야함
//             // int32 socket = channelUser->second.GetSocket();
//             // mNetwork.FetchToSendBuffer(socket,
//             //     ":"  + user.GetNickname() + "!" + user.GetUsername() + "@" + user.GetHostname()
//             //     + " " + "PART" + " " + channelName + "\r\n");
//             channelUser++;
//         }
//         mChannels[channelName].DeleteUser(user.GetNickname());
//         // TODO (bonus) : 채널에 유저가 없는 경우, 채널 삭제 (bonus : hello bot도 같이 없어져야함)
//         if (mChannels[channelName].IsChannelEmpty())
//         {
//             mChannels.erase(channelName);
//         }
//     }
// }

// void Core::processQUITMessage(const int32 IN socket,
//         const std::vector<std::string>& IN leading, const std::string& IN trailing)
// {
//     const std::string& command = leading[0];
//     const std::string& quitMessage = trailing;
//     const User& user = mUsers[socket];
//     LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
//         << " Quited : " << quitMessage << "(" << command << ")";
//     mNetwork.ClearRecvBuffer(socket);
//     mNetwork.DisconnectClient(socket);
// }

// void Core::processPINGMessage(const int32 IN socket, const std::vector<std::string>& IN leading)
// {
//     const std::string& pong("PONG");
//     const std::string& token = leading[1];
//     const User& user = mUsers[socket];
//     mNetwork.FetchToSendBuffer(socket, 
//             pong + " " + mNetwork.GetIPString(mNetwork.GetServerSocket()) + " :" + token + "\r\n");
//     LOG(LogLevel::Debug) << "Reply PONG to " << "[" << user.GetNickname() << "]";
// }

// void Core::processPONGMessage(const int32 IN socket, const std::vector<std::string>& IN leading)
// {
//     const std::string& token = leading[1];
//     User& user = mUsers[socket];
//     if (token == mNetwork.GetIPString(mNetwork.GetServerSocket()))
//     {
//         user.SetLastPongTime(time(NULL));
//     }
// }

}