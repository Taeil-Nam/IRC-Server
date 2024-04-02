#include "IRC.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"
#include "grc/irc/Channel.hpp"
#include "grc/irc/ChannelManager.hpp"

namespace grc
{

// mCommands
const std::string IRC::sStaticCommands[kIRCCommandSize]
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
        else if (command == sStaticCommands[kNick])
        {
            NICK(socket, command, parameters, trailing, password, network);
        }
        else if (command == sStaticCommands[kUser])
        {
            USER(socket, command, parameters, trailing, password, network);
        }
        else if (command == sStaticCommands[kQuit])
        {
            QUIT(socket, command, parameters, trailing, password, network);
        }
        else if (command == sStaticCommands[kJoin])
        {
            JOIN(socket, command, parameters, trailing, password, network);
        }
        else if (command == sStaticCommands[kPart])
        {
            PART(socket, command, parameters, trailing, password, network);
        }
        // else if (command == sStaticCommands[kMode])
        // {
        //     MODE(socket, command, parameters, trailing, password, network);
        // }
        // else if (command == sStaticCommands[kTopic])
        // {
        //     TOPIC(socket, command, parameters, trailing, password, network);
        // }
        // else if (command == sStaticCommands[kInvite])
        // {
        //     INVITE(socket, command, parameters, trailing, password, network);
        // }
        // else if (command == sStaticCommands[kKick])
        // {
        //     KICK(socket, command, parameters, trailing, password, network);
        // }
        else if (command == sStaticCommands[kPrivmsg])
        {
            PRIVMSG(socket, command, parameters, trailing, password, network);
        }
        else if (command == sStaticCommands[kPing])
        {
            PING(socket, command, parameters, trailing, password, network);
        }
        else if (command == sStaticCommands[kPong])
        {
            PONG(socket, command, parameters, trailing, password, network);
        }
   }
}

void IRC::parseMessage(const std::string& IN message,
                       std::string& OUT command,
                       std::vector<std::string>& OUT parameters,
                       std::string& OUT trailing)
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
        network.PushToSendBuffer(socket, messageToReply);
        // 즉시 메시지 전송 후, client 연결 해제
        network.SendToClient(socket);
        network.DisconnectClient(socket);
        UserManager::DeleteUser(socket);
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
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_PASSWDMISMATCH
    if (passwordInMessage != password)
    {
        messageToReply.append(ERR_PASSWDMISMATCH);
        messageToReply.append(" ");
        messageToReply.append(":Password incorrect");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        // 즉시 메시지 전송 후, client 연결 해제
        network.SendToClient(socket);
        network.DisconnectClient(socket);
        UserManager::DeleteUser(socket);
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
        network.PushToSendBuffer(socket, messageToReply);
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
        network.PushToSendBuffer(socket, messageToReply);
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
        network.PushToSendBuffer(socket, messageToReply);
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
        network.PushToSendBuffer(socket, messageToReply);
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
        network.PushToSendBuffer(socket, messageToReply);
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
    messageToReply.append("PING");
    messageToReply.append(" ");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(CRLF);
    network.PushToSendBuffer(socket, messageToReply);
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
    UserManager::DeleteUser(socket);
    ChannelManager::DeleteUserFromAllChannels(user);
    network.ClearRecvBuffer(socket);
    network.DisconnectClient(socket);
    LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
        << " Quited : " << quitMessage << "(" << command << ")";
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
        network.PushToSendBuffer(socket, messageToReply);
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
            network.PushToSendBuffer(socket, messageToReply);
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
                network.PushToSendBuffer(socket, messageToReply);
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
                    network.PushToSendBuffer(socket, messageToReply);
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
                network.PushToSendBuffer(socket, messageToReply);
                return;
            }
            // 채널에 유저 입장
            channel.AddUser(user.GetNickname(), user);
        }
        Channel& channel = ChannelManager::GetChannel(channelName);
        channel.AddUser(user.GetNickname(), user);
        // Notice
        messageToReply.append(":");
        messageToReply.append(user.GetNickname());
        messageToReply.append("!");
        messageToReply.append(user.GetUsername());
        messageToReply.append("@");
        messageToReply.append(user.GetHostname());
        messageToReply.append(" ");
        messageToReply.append("JOIN");
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(CRLF);
        // RPL_TOPIC
        messageToReply.append(RPL_TOPIC);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":");
        messageToReply.append(channel.GetTopic());
        messageToReply.append(CRLF);
        // RPL_NAMREPLY
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
        // RPL_ENDOFNAMES
        messageToReply.append(RPL_ENDOFNAMES);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":End of /NAMES list");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        messageToReply.clear();
        // 채널의 모든 유저에게 새로운 유저 입장 알림 (JOIN)
        messageToReply.append(":");
        messageToReply.append(user.GetNickname());
        messageToReply.append("!");
        messageToReply.append(user.GetUsername());
        messageToReply.append("@");
        messageToReply.append(user.GetHostname());
        messageToReply.append(" ");
        messageToReply.append("JOIN");
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(CRLF);
        std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
        while (userInChannel != channel.GetUsers().end())
        {
            if (userInChannel->second.GetNickname() == user.GetNickname())
            {
                userInChannel++;
                continue;
            }
            network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
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
        network.PushToSendBuffer(socket, messageToReply);
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
            network.PushToSendBuffer(socket, messageToReply);
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
            network.PushToSendBuffer(socket, messageToReply);
            continue;
        }
        // 채널의 모든 유저들한테 해당 유저 나감을 알림
        const std::string& reason = trailing;
        messageToReply.append(":");
        messageToReply.append(user.GetNickname());
        messageToReply.append("!");
        messageToReply.append(user.GetUsername());
        messageToReply.append("@");
        messageToReply.append(user.GetHostname());
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        if (reason.empty())
        {
            messageToReply.append(":");
            messageToReply.append(channelName);
        }
        else
        {
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":");
            messageToReply.append(reason);
        }
        messageToReply.append(CRLF);
        std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
        while (userInChannel != channel.GetUsers().end())
        {
            network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
            userInChannel++;
        }
        messageToReply.clear();
        channel.DeleteUser(user.GetNickname());
        // TODO (bonus) : 채널에 유저가 없는 경우, 채널 삭제 (bonus : hello bot도 같이 없어져야함)
        if (channel.IsChannelEmpty())
        {
            ChannelManager::DeleteChannel(channelName);
            LOG(LogLevel::Informational) << "Delete channel " << "[" << channelName << "]";
        }
    }
}

void IRC::PRIVMSG(const int32 IN socket, 
                  const std::string& IN command,
                  const std::vector<std::string>& IN parameters,
                  const std::string& IN trailing,
                  const std::string& IN password,
                  Network& IN OUT network)
{
    static_cast<void>(password);
    const User& user = UserManager::GetUser(socket);
    std::string messageToReply("");
    const std::string& target = parameters[0];
    bool IsChannel = false;
    if (target[0] == '&' || target[0] == '#')
    {
        IsChannel = true;
    }
    // ERR_NORECIPIENT
    if (parameters.size() < 1)
    {
        messageToReply.append(ERR_NORECIPIENT);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(":No recipient given");
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_NOTEXTTOSEND
    if (trailing.empty())
    {
        messageToReply.append(ERR_NOTEXTTOSEND);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(":No text to send");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_CANNOTSENDTOCHAN
    if (IsChannel
        && ChannelManager::GetChannel(target).IsUserExist(user.GetNickname()) == false)
    {
        messageToReply.append(ERR_CANNOTSENDTOCHAN);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(ChannelManager::GetChannel(target).GetName());
        messageToReply.append(" ");
        messageToReply.append(":Cannot send to channel");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_NOSUCHNICK
    if (IsChannel == false && UserManager::IsUserExist(target) == false)
    {
        messageToReply.append(ERR_NOSUCHNICK);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(target);
        messageToReply.append(" ");
        messageToReply.append(":No such nick/channel");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // target이 채널인 경우
    if (IsChannel)
    {
        const Channel& channel = ChannelManager::GetChannel(target);
        const std::map<std::string, User>& usersInChannel = channel.GetUsers();
        std::map<std::string, User>::const_iterator userInChannel = usersInChannel.begin();
        while (userInChannel != usersInChannel.end())
        {
            if (userInChannel->second.GetNickname() == user.GetNickname())
            {
                userInChannel++;
                continue;
            }
            messageToReply.append(":");
            messageToReply.append(user.GetNickname());
            messageToReply.append(" ");
            messageToReply.append(command);
            messageToReply.append(" ");
            messageToReply.append(target);
            messageToReply.append(" :");
            messageToReply.append(trailing);
            messageToReply.append(CRLF);
            network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
            messageToReply.clear();
            userInChannel++;
        }
    }
    // target이 유저인 경우
    else
    {
        const User& targetUser = UserManager::GetUser(target);
        messageToReply.append(":");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(target);
        messageToReply.append(" :");
        messageToReply.append(trailing);
        messageToReply.append(CRLF);
        network.PushToSendBuffer(targetUser.GetSocket(), messageToReply);
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
    network.PushToSendBuffer(socket, messageToReply);
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
    if (user.IsRegistered() == false && token == network.GetIPString(network.GetServerSocket()))
    {
        user.SetRegistered();
    }
    SendWelcomeMessage(socket, network);
}

void IRC::SendWelcomeMessage(const int32 IN socket, Network& IN OUT network)
{
    const User& user = UserManager::GetUser(socket);
    std::string messageToReply("");
    // RPL_WELCOME
    messageToReply.append(":");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(RPL_WELCOME);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append(":Welcome to the GameRelayChat Network,");
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append("!");
    messageToReply.append(user.GetUsername());
    messageToReply.append("@");
    messageToReply.append(user.GetHostname());
    messageToReply.append(CRLF);
    // RPL_YOURHOST
    messageToReply.append(":");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(RPL_YOURHOST);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append(":Your host is");
    messageToReply.append(" ");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(", running version ");
    messageToReply.append(IRC_VERSION);
    messageToReply.append(CRLF);
    // RPL_CREATED
    messageToReply.append(":");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(RPL_CREATED);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append(":GameRC was created 2024");
    messageToReply.append(" ");
    messageToReply.append(CRLF);
    // RPL_MYINFO
    messageToReply.append(":");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(RPL_MYINFO);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(IRC_VERSION);
    messageToReply.append(" ");
    messageToReply.append(CRLF);
    // RPL_ISUPPORT
    messageToReply.append(":");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(RPL_ISUPPORT);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append("MAXNICKLEN=9");
    messageToReply.append(" ");
    messageToReply.append("MAXCHANNELLEN=200");
    messageToReply.append(" ");
    messageToReply.append(":are supported by this server");
    messageToReply.append(CRLF);
    // ERR_NOMOTD
    messageToReply.append(":");
    messageToReply.append(network.GetIPString(network.GetServerSocket()));
    messageToReply.append(" ");
    messageToReply.append(ERR_NOMOTD);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append(":MOTD File is missing");
    messageToReply.append(CRLF);
    network.PushToSendBuffer(socket, messageToReply);
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