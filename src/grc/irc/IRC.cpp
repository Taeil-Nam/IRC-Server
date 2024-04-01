#include "IRC.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"
#include "grc/irc/Channel.hpp"

namespace grc
{

const std::string IRC::mCommands[kIRCCommandSize]
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

void IRC::HandleMessage(const int32 IN socket, Network& IN network, const std::string& IN password)
{
    const User& user = UserManager::GetUser(socket);
    std::string message;
    while (network.PullFromRecvBuffer(socket, message, CRLF))
    {
        if (message.size() > 510)
        {
            continue;
        }
        std::string command("");
        std::vector<std::string> parameters;
        std::string trailing("");
        parseMessage(message, command, parameters, trailing);
        if (user.IsAuthenticated() == false)
        {
            PASS(socket, command, parameters, trailing, password, network);
        }
        else if (command == mCommands[kNick])
        {
            NICK(socket, command, parameters, trailing, password, network);
        }
        else if (command == mCommands[kUser])
        {
            USER(socket, command, parameters, trailing, password, network);
        }
        else if (command == mCommands[kQuit])
        {
            QUIT(socket, command, parameters, trailing, password, network);
        }
        else if (command == mCommands[kJoin])
        {
            JOIN(socket, command, parameters, trailing, password, network);
        }
        else if (command == mCommands[kPart])
        {
            PART(socket, command, parameters, trailing, password, network);
        }
        // else if (command == mCommands[kMode])
        // {
        //     MODE(socket, command, parameters, trailing, password, network);
        // }
        // else if (command == mCommands[kTopic])
        // {
        //     TOPIC(socket, command, parameters, trailing, password, network);
        // }
        // else if (command == mCommands[kInvite])
        // {
        //     INVITE(socket, command, parameters, trailing, password, network);
        // }
        // else if (command == mCommands[kKick])
        // {
        //     KICK(socket, command, parameters, trailing, password, network);
        // }
        // else if (command == mCommands[kPrivmsg])
        // {
        //     PRIVMSG(socket, command, parameters, trailing, password, network);
        // }
        else if (command == mCommands[kPing])
        {
            PING(socket, command, parameters, trailing, password, network);
        }
        else if (command == mCommands[kPong])
        {
            PONG(socket, command, parameters, trailing, password, network);
        }
   }
}

void IRC::parseMessage(const std::string& IN message,
                       std::string& OUT command,
                       std::vector<std::string>& OUT parameters,
                       std::string OUT trailing)
{
    // [Input message format]
    // <command> <parameters> <trailing>
    // <trailing> = ':'로 시작하는 맨 마지막 문자열.
    // USER message ex: USER tnamUser tnamUser localhost :tnamReal
    std::vector<std::string> tokens = split(message, ":");
    if (tokens.size() >= 2)
    {
        trailing = tokens[1];
    }
    std::vector<std::string> leading = split(tokens[0], " ");
    for (std::size_t i = 0; i < leading.size(); i++)
    {
        if (i == 0)
        {
            command = leading[i];
            continue;
        }
        parameters.push_back(leading[i]);
    }
}

void IRC::PASS(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)trailing;
    std::string messageToReply("");
    if (command != "PASS")
    {
        network.ClearRecvBuffer(socket);
        network.DisconnectClient(socket);
        return;
    }
    // ERR_NEEDMOREPARAMS
    if (parameters.size()  < 1)
    {
        messageToReply.append(ERR_NEEDMOREPARAMS);
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(":Not enough parameters");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        // 즉시 메시지 전송 후, client 연결 해제
        network.SendToClient(socket);
        network.DisconnectClient(socket);
        return;
    }
    const std::string& passwordInMessage = parameters[0];
    User& user = UserManager::GetUser(socket);
    // ERR_ALREADYREGISTERED
    if (user.IsAuthenticated())
    {
        messageToReply.append(ERR_ALREADYREGISTERED);
        messageToReply.append(" ");
        messageToReply.append(":You may not reregister");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_PASSWDMISMATCH
    if (passwordInMessage != password)
    {
        messageToReply.append(ERR_PASSWDMISMATCH);
        messageToReply.append(" ");
        messageToReply.append(":Password incorrect");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        // 즉시 메시지 전송 후, client 연결 해제
        network.SendToClient(socket);
        network.DisconnectClient(socket);
        return;
    }
    user.SetAuthenticated();
}

void IRC::NICK(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)command;
    (void)trailing;
    (void)password;
    std::string messageToReply("");
    // ERR_NONICKNAMEGIVEN
    if (parameters.size() < 1)
    {
        messageToReply.append(ERR_NONICKNAMEGIVEN);
        messageToReply.append(" ");
        messageToReply.append(":No nickname given");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    const std::string& nickname = parameters[0];
    // ERR_ERRONEUSNICKNAME
    if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '&' || nickname[0] == '#'
        || nickname.find(' ') != std::string::npos || nickname.find(',') != std::string::npos
        || nickname.find('*') != std::string::npos || nickname.find('?') != std::string::npos
        || nickname.find('!') != std::string::npos || nickname.find('@') != std::string::npos
        || nickname.find('.') != std::string::npos || nickname.size() > 9)
    {
        messageToReply.append(ERR_ERRONEUSNICKNAME);
        messageToReply.append(" ");
        messageToReply.append(nickname);
        messageToReply.append(" ");
        messageToReply.append(":Erroneus nickname");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_NICKNAMEINUSE
    if (isNicknameInUse(nickname))
    {
        messageToReply.append(ERR_NICKNAMEINUSE);
        messageToReply.append(" ");
        messageToReply.append(nickname);
        messageToReply.append(" ");
        messageToReply.append(":");
        messageToReply.append(nickname);
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    User& user = UserManager::GetUser(socket);
    user.SetNickname(nickname);
}

void IRC::USER(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)password;
    std::string messageToReply("");
    // ERR_NEEDMOREPARAMS
    if (parameters.size() < 4)
    {
        messageToReply.append(ERR_NEEDMOREPARAMS);
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(":Not enough parameters");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    User& user = UserManager::GetUser(socket);
    // ERR_ALREADYREGISTERED
    if (user.IsRegistered())
    {
        messageToReply.append(ERR_ALREADYREGISTERED);
        messageToReply.append(" ");
        messageToReply.append(":You may not reregister");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    const std::string& username = parameters[0];
    const std::string& hostname = parameters[1];
    const std::string& servername = parameters[2];
    const std::string& realname = trailing;
    user.SetUsername(username);
    user.SetHostname(hostname);
    user.SetServername(servername);
    user.SetRealname(realname);
    messageToReply.append(RPL_WELCOME);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append(":Welcome to the GameRelayChat Network,");
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append("!");
    messageToReply.append(username);
    messageToReply.append("@");
    messageToReply.append(hostname);
    messageToReply.append(CRLF);
    messageToReply.append("PING");
    messageToReply.append(" ");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(CRLF);
    network.FetchToSendBuffer(socket, messageToReply);
}

void IRC::QUIT(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)parameters;
    (void)password;
    const std::string& quitMessage = trailing;
    const User& user = UserManager::GetUser(socket);
    LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
        << " Quited : " << quitMessage << "(" << command << ")";
    network.ClearRecvBuffer(socket);
    network.DisconnectClient(socket);
}

void IRC::JOIN(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)trailing;
    (void)password;
    std::string messageToReply("");
    // ERR_NEEDMOREPARAMS
    if (parameters.size() < 1)
    {
        messageToReply.append(ERR_NEEDMOREPARAMS);
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(":Not enough parameters");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    const std::vector<std::string> channels = split(parameters[0], ",");
    std::vector<std::string> keys;
    if (parameters.size() >= 2)
    {
        keys = split(parameters[1], ",");
    }

    const User& user = UserManager::GetUser(socket);
    for (std::size_t i = 0; i < channels.size(); i++)
    {
        const std::string& channelName = channels[i];
        // ERR_NOSUCHCHANNEL
        if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
            || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos)
        {
            messageToReply.append(ERR_NOSUCHCHANNEL);
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":No such channel");
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            return;
        }
        std::string key;
        if (i < keys.size())
        {
            key = keys[i];
        }
        // 채널이 새로 생성된 경우
        if (ChannelManager::IsChannelExist(channelName) == 0)
        {
            ChannelManager::AddChannel(channelName);
            Channel& channel = ChannelManager::GetChannel(channelName);
            channel.AddUser(user.GetNickname(), user);
            channel.AddOperator(user.GetNickname(), user);
            // TODO : hello bot(client)를 해당 채널에 추가(bonus)
            LOG(LogLevel::Informational) << "Create new channel " << "[" << channelName << "]";
            LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
                << " join to channel " << "[" << channelName << "]";
            LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
                << " is now operator of channel " << "[" << channelName << "]";
        }
        // 기존 채널에 입장하는 경우
        else
        {
            Channel& channel = ChannelManager::GetChannel(channelName);
            // ERR_INVITEONLYCHAN
            if (channel.IsInviteOnly())
            {
                messageToReply.append(ERR_INVITEONLYCHAN);
                messageToReply.append(" ");
                messageToReply.append(channelName);
                messageToReply.append(" ");
                messageToReply.append(":Cannot join channel (+i)");
                messageToReply.append(CRLF);
                network.FetchToSendBuffer(socket, messageToReply);
                return;
            }
            // ERR_BADCHANNELKEY
            if (channel.IsKeyRequired())
            {
                if (key != channel.GetKey())
                {
                    messageToReply.append(ERR_BADCHANNELKEY);
                    messageToReply.append(" ");
                    messageToReply.append(channelName);
                    messageToReply.append(" ");
                    messageToReply.append(":Cannot join channel (+k)");
                    messageToReply.append(CRLF);
                    network.FetchToSendBuffer(socket, messageToReply);
                    return;
                }
            }
            // ERR_CHANNELISFULL
            if (channel.GetMaxUserCount() != Channel::UNLIMIT
                && channel.GetCurrentUserCount() >= channel.GetMaxUserCount())
            {
                messageToReply.append(ERR_CHANNELISFULL);
                messageToReply.append(" ");
                messageToReply.append(channelName);
                messageToReply.append(" ");
                messageToReply.append(":Cannot join channel (+l)");
                messageToReply.append(CRLF);
                network.FetchToSendBuffer(socket, messageToReply);
                return;
            }
            // 채널에 유저 입장
            channel.AddUser(user.GetNickname(), user);
        }
        // 채널의 모든 유저에게 새로운 유저 입장 알림
        Channel& channel = ChannelManager::GetChannel(channelName);
        std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
        while (userInChannel != channel.GetUsers().end())
        {
            int32 socket = userInChannel->second.GetSocket();
            messageToReply.append(":");
            messageToReply.append(user.GetNickname());
            messageToReply.append(" ");
            messageToReply.append("JOIN");
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            messageToReply.clear();

            messageToReply.append(RPL_TOPIC);
            messageToReply.append(" ");
            messageToReply.append(user.GetNickname());
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":");
            messageToReply.append(channel.GetTopic());
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            messageToReply.clear();

            messageToReply.append(RPL_NAMREPLY);
            messageToReply.append(" ");
            messageToReply.append(user.GetNickname());
            messageToReply.append(" ");
            messageToReply.append("=");
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":");
            messageToReply.append(channel.GetAllUsersNickname());
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            messageToReply.clear();

            messageToReply.append(RPL_ENDOFNAMES);
            messageToReply.append(" ");
            messageToReply.append(user.GetNickname());
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":End of /NAMES list");
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            userInChannel++;
        }
    }
}

void IRC::PART(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)password;
    std::string messageToReply("");
    // ERR_NEEDMOREPARAMS
    if (parameters.size() < 1)
    {
        messageToReply.append(ERR_NEEDMOREPARAMS);
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(":Not enough parameters");
        messageToReply.append(CRLF);
        network.FetchToSendBuffer(socket, messageToReply);
        return;
    }
    const std::vector<std::string> channels = split(parameters[0], ",");
    for (std::size_t i = 0; i < channels.size(); i++)
    {
        messageToReply.clear();
        const std::string& channelName = channels[i];
        // ERR_NOSUCHCHANNEL
        if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
            || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos
            || ChannelManager::IsChannelExist(channelName) == 0)
        {
            messageToReply.append(ERR_NOSUCHCHANNEL);
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":No such channel");
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            continue;
        }
        // ERR_NOTONCHANNEL
        Channel& channel = ChannelManager::GetChannel(channelName);
        const User& user = UserManager::GetUser(socket);
        if (channel.IsUserExist(user.GetNickname()) == false)
        {
            messageToReply.append(ERR_NOTONCHANNEL);
            messageToReply.append(" ");
            messageToReply.append(user.GetNickname());
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(":You're not on that channel");
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            continue;
        }
        // 채널의 모든 유저들한테 해당 유저 나감을 알림
        std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
        while (userInChannel != channel.GetUsers().end())
        {
            // TODO : 응답 메시지 형식 알아낸 후 보내기
            // :taeil!~taeil@localhost PART :#gdf
            const std::string& reason = trailing;
            messageToReply.append(":");
            messageToReply.append(user.GetNickname());
            messageToReply.append("!~");
            messageToReply.append(user.GetUsername());
            messageToReply.append("@");
            messageToReply.append("127.0.0.1");
            messageToReply.append(" ");
            messageToReply.append(command);
            messageToReply.append(" ");
            messageToReply.append(":");
            messageToReply.append(channelName);
            if (reason != "")
            {
                messageToReply.append("-");
                messageToReply.append(reason);
            }
            messageToReply.append(CRLF);
            network.FetchToSendBuffer(socket, messageToReply);
            userInChannel++;
        }
        channel.DeleteUser(user.GetNickname());
        // TODO (bonus) : 채널에 유저가 없는 경우, 채널 삭제 (bonus : hello bot도 같이 없어져야함)
        if (channel.IsChannelEmpty())
        {
            ChannelManager::DeleteChannel(channelName);
        }
    }
}

void IRC::PING(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)command;
    (void)trailing;
    (void)password;
    const std::string& token = parameters[0];
    std::string messageToReply("");
    messageToReply.append("PONG");
    messageToReply.append(" ");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(":");
    messageToReply.append(token);
    messageToReply.append(CRLF);
    network.FetchToSendBuffer(socket, messageToReply);
}

void IRC::PONG(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    (void)command;
    (void)trailing;
    (void)password;
    const std::string& token = parameters[0];
    User& user = UserManager::GetUser(socket);
    if (token == network.GetIPString(network.GetServerSocket()))
    {
        user.SetRegistered();
    }
}

bool IRC::isNicknameInUse(const std::string& IN nickName)
{
    std::map<int32, User>::const_iterator it = UserManager::GetUsers().begin();
    while (it != UserManager::GetUsers().end())
    {
        if (it->second.GetNickname() == nickName)
        {
            return true;
        }
        it++;
    }
    return false;
}

}