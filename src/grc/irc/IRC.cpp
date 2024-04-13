#include "IRC.hpp"
#include "BSD-GDF/Assert.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"
#include <cctype>

namespace grc
{

std::map<std::string, IRC::TcommandFunctionPTR> IRC::sStaticCommandFunctionMap;

void IRC::HandleMessage(const int32 IN socket, Network& IN network, const std::string& IN password)
{
    if (sStaticCommandFunctionMap.empty())
        initializeCommandFunctionMap();
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
        else
        {
            std::map<std::string, TcommandFunctionPTR>::const_iterator it;
            it = sStaticCommandFunctionMap.find(command);
            if (it != sStaticCommandFunctionMap.end())
            {
                it->second(socket,
                           command,
                           parameters,
                           trailing,
                           password,
                           network);
            }
        }
   }
}

void IRC::initializeCommandFunctionMap()
{
    sStaticCommandFunctionMap["PASS"] = &PASS;
    sStaticCommandFunctionMap["NICK"] = &NICK;
    sStaticCommandFunctionMap["USER"] = &USER;
    sStaticCommandFunctionMap["QUIT"] = &QUIT;
    sStaticCommandFunctionMap["JOIN"] = &JOIN;
    sStaticCommandFunctionMap["PART"] = &PART;
    sStaticCommandFunctionMap["MODE"] = &MODE;
    sStaticCommandFunctionMap["TOPIC"] = &TOPIC;
    sStaticCommandFunctionMap["INVITE"] = &INVITE;
    sStaticCommandFunctionMap["KICK"] = &KICK;
    sStaticCommandFunctionMap["PRIVMSG"] = &PRIVMSG;
    sStaticCommandFunctionMap["PING"] = &PING;
    sStaticCommandFunctionMap["PONG"] = &PONG;
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
    // TOPIC 전용 로직 (TOPIC은 trailing이 빈 문자열인지 구분하는게 필요함)
    if (command == "TOPIC" && trailing.empty()
        && message.find(":") != std::string::npos)
    {
        trailing = ":";
    }
}

void IRC::PASS(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    static_cast<void>(trailing);
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
        network.ReserveDisconnectClient(socket);
        network.ClearRecvBuffer(socket);
        UserManager::DeleteUser(socket);
        return;
    }
    const std::string& passwordInMessage = parameters[0];
    // ERR_PASSWDMISMATCH
    if (passwordInMessage != password)
    {
        messageToReply.append(ERR_PASSWDMISMATCH);
        messageToReply.append(" ");
        messageToReply.append(":Password incorrect");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        network.ReserveDisconnectClient(socket);
        network.ClearRecvBuffer(socket);
        UserManager::DeleteUser(socket);
        return;
    }
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
    user.SetAuthenticated();
}

void IRC::NICK(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    static_cast<void>(command);
    static_cast<void>(trailing);
    static_cast<void>(password);
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
    // 새로운 유저인 경우
    if (UserManager::GetUser(socket).GetNickname().empty())
    {
        UserManager::GetUser(socket).SetNickname(nickname);
        LOG(LogLevel::Informational) << "New user " << "[" << nickname << "]"
            << " Created" << "(" << command << ")";
    }
    // 유저가 존재하는 경우 닉네임 변경 및 채널에 알림
    else
    {
        const std::string oldNickname = UserManager::GetUser(socket).GetNickname();
        UserManager::GetUser(socket).SetNickname(nickname);
        messageToReply.append(":");
        messageToReply.append(oldNickname);
        messageToReply.append("!");
        messageToReply.append(UserManager::GetUser(socket).GetUsername());
        messageToReply.append("@");
        messageToReply.append(UserManager::GetUser(socket).GetHostname());
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(nickname);
        messageToReply.append(CRLF);
        std::map<std::string, Channel>::iterator channelIt = ChannelManager::GetChannels().begin();
        while (channelIt != ChannelManager::GetChannels().end())
        {
            Channel& channel = channelIt->second;
            if (channel.IsUserExist(oldNickname) == false)
            {
                channelIt++;
                continue;
            }
            std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
            while (userInChannel != channel.GetUsers().end())
            {
                if (oldNickname == userInChannel->second.GetNickname())
                {
                    userInChannel++;
                    continue;
                }
                network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
                userInChannel++;
            }
            channel.AddUser(nickname, UserManager::GetUser(socket));
            if (channel.IsOperator(oldNickname))
            {
                channel.AddOperator(nickname, UserManager::GetUser(socket));
            }
            channel.DeleteUser(oldNickname);
            channelIt++;
        }
        network.PushToSendBuffer(socket, messageToReply);
        LOG(LogLevel::Informational) << "User " << "[" << oldNickname << "]"
            << " change nickname to " << "[" << UserManager::GetUser(socket).GetNickname() << "]" << "(" << command << ")";
    }
}

void IRC::USER(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    static_cast<void>(password);
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
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(parameters);
    static_cast<void>(password);
    const std::string& quitMessage = trailing;
    const User& user = UserManager::GetUser(socket);
    // 유저가 속한 채널에 알림
    std::string messageToReply("");
    messageToReply.append(":");
    messageToReply.append(user.GetNickname());
    messageToReply.append("!");
    messageToReply.append(user.GetUsername());
    messageToReply.append("@");
    messageToReply.append(user.GetHostname());
    messageToReply.append(" ");
    messageToReply.append(command);
    messageToReply.append(" ");
    messageToReply.append(":");
    messageToReply.append(command);
    messageToReply.append(":");
    messageToReply.append(" ");
    messageToReply.append(quitMessage);
    messageToReply.append(CRLF);
    std::map<std::string, Channel>::iterator channelIt = ChannelManager::GetChannels().begin();
    while (channelIt != ChannelManager::GetChannels().end())
    {
        Channel& channel = channelIt->second;
        if (channel.IsUserExist(user.GetNickname()) == false)
        {
            channelIt++;
            continue;
        }
        std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
        while (userInChannel != channel.GetUsers().end())
        {
            network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
            userInChannel++;
        }
        // TODO (bonus) : 채널에 유저가 없는 경우, bot 삭제
        channelIt++;
        channel.DeleteUser(user.GetNickname());
        channel.DeleteOperator(user.GetNickname());
        channel.DeleteInvitedUser(user.GetNickname());
        ChannelManager::CheckIsEmptyChannelAndDelete(channel);
    }
    LOG(LogLevel::Informational) << "User " << "[" << user.GetNickname() << "]"
        << " Quited : " << quitMessage << "(" << command << ")";
    UserManager::DeleteUser(socket);
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
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(trailing);
    static_cast<void>(password);
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

    for (std::size_t i = 0; i < channels.size(); i++)
    {
        std::string channelName = channels[i];
        for (std::size_t j = 1; j < channelName.size(); j++)
        {
            channelName[j] = std::tolower(channelName[j]);
        }
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
        const User& user = UserManager::GetUser(socket);
        if (ChannelManager::IsChannelExist(channelName) == false)
        {
            ChannelManager::AddChannel(channelName);
            Channel& channel = ChannelManager::GetChannel(channelName);
            channel.AddUser(user.GetNickname(), user);
            channel.AddOperator(user.GetNickname(), user);
        }
        // 기존 채널에 입장하는 경우
        else
        {
            Channel& channel = ChannelManager::GetChannel(channelName);
            // ERR_INVITEONLYCHAN
            if (channel.IsInviteOnly() && channel.IsInvited(user.GetNickname()) == false)
            {
                messageToReply.append(ERR_INVITEONLYCHAN);
                messageToReply.append(" ");
                messageToReply.append(user.GetNickname());
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
                    messageToReply.append(user.GetNickname());
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
                messageToReply.append(user.GetNickname());
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
        messageToReply.append(command);
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
        messageToReply.append(command);
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
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(password);
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
        const std::string& channelName = channels[i];
        // ERR_NOSUCHCHANNEL
        if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
            || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos
            || ChannelManager::IsChannelExist(channelName) == false)
        {
            messageToReply.append(ERR_NOSUCHCHANNEL);
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":No such channel");
            messageToReply.append(CRLF);
            network.PushToSendBuffer(socket, messageToReply);
            messageToReply.clear();
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
            messageToReply.clear();
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
        // TODO (bonus) : 채널에 유저가 없는 경우, bot 삭제
        channel.DeleteUser(user.GetNickname());
        channel.DeleteOperator(user.GetNickname());
        channel.DeleteInvitedUser(user.GetNickname());
        ChannelManager::CheckIsEmptyChannelAndDelete(channel);
    }
}

void IRC::MODE(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(trailing);
    static_cast<void>(password);

    // mode message format
    // MODE     <channel>    <modestring>    <mode argument>
    // command | param[0]     param[1]         param[2]
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
    const std::string channelName = parameters[0];
    // 유저 대상인 경우 생략
    if ((channelName[0] != '#' && channelName[0] != '&'))
    {
        return;
    }
    // ERR_NOSUCHCHANNEL
    if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
            || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos
            || ChannelManager::IsChannelExist(channelName) == false)
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
    // modestring이 없는 경우, 해당 채널의 모드 확인만 함.
    // RPL_CHANNELMODEIS
    Channel& channel = ChannelManager::GetChannel(channelName);
    const User& user = UserManager::GetUser(socket);
    if (parameters.size() < 2)
    {
        // RPL_CHANNELMODEIS
        messageToReply.append(RPL_CHANNELMODEIS);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(channel.GetModeString());
        messageToReply.append(" ");
        messageToReply.append(channel.GetModeArgument());
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    const std::string& modeString = parameters[1];
    // 지원하지 않는 옵션(밴 리스트 출력)인 경우 생략
    if (modeString[0] == 'b')
    {
        return;
    }
    // modestring이 있는데 채널의 오퍼레이터가 아닌 경우, ERR_CHANOPRIVSNEEDED 응답
    // ERR_CHANOPRIVSNEEDED
    if (ChannelManager::GetChannel(channelName).IsOperator(user.GetNickname()) == false)
    {
        messageToReply.append(ERR_CHANOPRIVSNEEDED);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":You're not channel operator");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // MODE 설정 또는 해제
    std::vector<std::string> modeArgument;
    if (parameters.size() >= 3)
    {
        for (std::size_t i = 2; i < parameters.size(); i++)
        {
            modeArgument.push_back(parameters[i]);
        }
    }
    std::string resultModeString("");
    std::string resultArgumentString("");
    std::size_t modeStringIndex = 0;
    std::size_t modeArgumentIndex = 0;
    bool isAddType = false;
    while (modeStringIndex < modeString.size())
    {
        switch (modeString[modeStringIndex])
        {
        case '+':
            isAddType = true;
            resultModeString.append("+");
            break;
        case '-':
            isAddType = false;
            resultModeString.append("-");
            break;
        // argument의 닉네임에게 operator 권한 주기.
        case 'o':
            if (isAddType)
            {
                if (modeArgumentIndex >= modeArgument.size())
                {
                    break;
                }
                if (channel.IsUserExist(modeArgument[modeArgumentIndex]) == false)
                {
                    modeArgumentIndex++;
                    break;
                }
                channel.AddOperator(modeArgument[modeArgumentIndex],
                               UserManager::GetUser(modeArgument[modeArgumentIndex]));
            }
            else
            {
                if (modeArgumentIndex >= modeArgument.size())
                {
                    break;
                }
                if (channel.IsUserExist(modeArgument[modeArgumentIndex]) == false)
                {
                    modeArgumentIndex++;
                    break;
                }
                channel.DeleteOperator(modeArgument[modeArgumentIndex]);
            }
            resultModeString.append("o");
            resultArgumentString.append(modeArgument[modeArgumentIndex]);
            resultArgumentString.append(" ");
            modeArgumentIndex++;
            break;
        // 토픽을 오퍼레이터만 변경할 수 있도록 함.
        case 't':
            if (isAddType)
            {
                channel.SetProtectedTopic();
            }
            else
            {
                channel.UnsetProtectedTopic();
            }
            resultModeString.append("t");
            break;
        // 채널을 초대 전용으로 설정.
        case 'i':
            if (isAddType)
            {
                channel.SetInviteOnly();
            }
            else
            {
                channel.UnsetInviteOnly();
            }
            resultModeString.append("i");
            break;
        // 채널의 최대 유저 수를 설정.
        case 'l':
            if (isAddType)
            {
                if (modeArgumentIndex >= modeArgument.size())
                {
                    break;
                }
                channel.SetLimitedMaxUserCount();
                int maxUserCount = std::atoi(modeArgument[modeArgumentIndex].c_str());
                if (maxUserCount == Channel::UNLIMIT)
                {
                    modeArgumentIndex++;
                    break;
                }
                channel.SetMaxUserCount(maxUserCount);
                resultArgumentString.append(std::to_string(maxUserCount));
                resultArgumentString.append(" ");
                modeArgumentIndex++;
            }
            else
            {
                channel.UnsetLimitedMaxUserCount();
                channel.SetMaxUserCount(Channel::UNLIMIT);
            }
            resultModeString.append("l");
            break;
        // 채널의 key를 설정.
        case 'k':
            if (isAddType)
            {
                if (modeArgumentIndex >= modeArgument.size())
                {
                    break;
                }
                channel.SetKeyRequired();
                channel.SetKey(modeArgument[modeArgumentIndex]);
                resultArgumentString.append(modeArgument[modeArgumentIndex]);
                resultArgumentString.append(" ");
                modeArgumentIndex++;
            }
            else
            {
                channel.UnsetKeyRequired();
                channel.SetKey("");
            }
            resultModeString.append("k");
            break;
        default:
            break;
        }
        modeStringIndex++;
    }
    // 채널의 모든 유저에게 알림
    messageToReply.append(":");
    messageToReply.append(user.GetNickname());
    messageToReply.append("!");
    messageToReply.append(user.GetUsername());
    messageToReply.append("@");
    messageToReply.append(user.GetHostname());
    messageToReply.append(" ");
    messageToReply.append(command);
    messageToReply.append(" ");
    messageToReply.append(channelName);
    messageToReply.append(" ");
    messageToReply.append(resultModeString);
    messageToReply.append(" ");
    messageToReply.append(resultArgumentString);
    messageToReply.pop_back(); // 맨 끝의 공백 문자 제거
    messageToReply.append(CRLF);
    // 보낼 메시지가 없는 경우 생략
    if (messageToReply == "CRLF")
    {
        return;
    }
    std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
    while (userInChannel != channel.GetUsers().end())
    {
        network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
        userInChannel++;
    }
}

void IRC::TOPIC(const int32 IN socket,
                const std::string& IN command,
                const std::vector<std::string>& IN parameters,
                const std::string& IN trailing,
                const std::string& IN password,
                Network& IN OUT network)
{
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(password);
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
    // ERR_NOSUCHCHANNEL
    const std::string& channelName = parameters[0];
    if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
        || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos
        || ChannelManager::IsChannelExist(channelName) == false)
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
        return;
    }
    // ERR_CHANOPRIVSNEEDED
    if (ChannelManager::GetChannel(channelName).IsProtectedTopic()
        && ChannelManager::GetChannel(channelName).IsOperator(user.GetNickname()) == false)
    {
        messageToReply.append(ERR_CHANOPRIVSNEEDED);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":You're not channel operator");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // 채널의 토픽을 설정하려는 경우
    if (trailing.empty() == false)
    {
        // 토픽 삭제하기
        if (trailing == ":")
        {
            channel.DeleteTopic();
        }
        // 토픽 변경하기
        else
        {
            channel.SetTopic(trailing);
        }
        // 채널의 모든 유저에게 알림
        messageToReply.append(":");
        messageToReply.append(user.GetNickname());
        messageToReply.append("!");
        messageToReply.append(user.GetUsername());
        messageToReply.append("@");
        messageToReply.append(user.GetHostname());
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":");
        messageToReply.append(channel.GetTopic());
        messageToReply.append(CRLF);
        std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
        while (userInChannel != channel.GetUsers().end())
        {
            network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
            userInChannel++;
        }
    }
    // 채널의 토픽을 확인하려는 경우
    else
    {
        // 채널에 토픽이 설정되어 있는 경우
        if (channel.GetTopic().empty() == false)
        {
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
        }
        // 채널에 토픽이 설정되어 있지 않은 경우
        else
        {
            // RPL_NOTOPIC
            messageToReply.append(RPL_NOTOPIC);
            messageToReply.append(" ");
            messageToReply.append(user.GetNickname());
            messageToReply.append(" ");
            messageToReply.append(channelName);
            messageToReply.append(" ");
            messageToReply.append(":No topic is set");
            messageToReply.append(CRLF);
        }
        network.PushToSendBuffer(socket, messageToReply);
    }
}

void IRC::INVITE(const int32 IN socket,
                 const std::string& IN command,
                 const std::vector<std::string>& IN parameters,
                 const std::string& IN trailing,
                 const std::string& IN password,
                 Network& IN OUT network)
{
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(trailing);
    static_cast<void>(password);
    std::string messageToReply("");
    // ERR_NEEDMOREPARAMS
    if (parameters.size() < 2)
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
    const std::string& targetUser = parameters[0];
    if (UserManager::IsUserExist(targetUser) == false)
    {
        return;
    }
    const std::string& channelName = parameters[1];
    // ERR_NOSUCHCHANNEL
    if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
        || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos
        || ChannelManager::IsChannelExist(channelName) == false)
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
    // ERR_NOTONCHANNEL
    const User& user = UserManager::GetUser(socket);
    const Channel& channel = ChannelManager::GetChannel(channelName);
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
        return;
    }
    // ERR_CHANOPRIVSNEEDED
    if (ChannelManager::GetChannel(channelName).IsOperator(user.GetNickname()) == false)
    {
        messageToReply.append(ERR_CHANOPRIVSNEEDED);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":You're not channel operator");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_USERONCHANNEL
    if (ChannelManager::GetChannel(channelName).IsUserExist(targetUser))
    {
        messageToReply.append(ERR_USERONCHANNEL);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(targetUser);
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":is already on channel");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // 채널의 초대 리스트에 추가
    ChannelManager::GetChannel(channelName).AddInvitedUser(targetUser, UserManager::GetUser(targetUser));
    // 초대한 사람에게 RPL_INVITING 응답
    messageToReply.append(RPL_INVITING);
    messageToReply.append(" ");
    messageToReply.append(user.GetNickname());
    messageToReply.append(" ");
    messageToReply.append(targetUser);
    messageToReply.append(" ");
    messageToReply.append(channelName);
    messageToReply.append(CRLF);
    network.PushToSendBuffer(socket, messageToReply);
    messageToReply.clear();
    // 초대 받은 사람에게 INVITE 메시지 전송
    messageToReply.append(":");
    messageToReply.append(user.GetNickname());
    messageToReply.append("!");
    messageToReply.append(user.GetUsername());
    messageToReply.append("@");
    messageToReply.append(user.GetHostname());
    messageToReply.append(" ");
    messageToReply.append(command);
    messageToReply.append(" ");
    messageToReply.append(targetUser);
    messageToReply.append(" ");
    messageToReply.append(channelName);
    messageToReply.append(CRLF);
    network.PushToSendBuffer(UserManager::GetUser(targetUser).GetSocket(), messageToReply);
}

void IRC::KICK(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(password);
    std::string messageToReply("");
    // ERR_NEEDMOREPARAMS
    if (parameters.size() < 2)
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
    const std::string& channelName = parameters[0];
    // ERR_NOSUCHCHANNEL
    if ((channelName[0] != '#' && channelName[0] != '&') || channelName.find(' ') != std::string::npos
        || channelName.find(',') != std::string::npos || channelName.find(7) != std::string::npos
        || ChannelManager::IsChannelExist(channelName) == false)
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
    // ERR_CHANOPRIVSNEEDED
    const User& user = UserManager::GetUser(socket);
    if (ChannelManager::GetChannel(channelName).IsOperator(user.GetNickname()) == false)
    {
        messageToReply.append(ERR_CHANOPRIVSNEEDED);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":You're not channel operator");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_USERNOTINCHANNEL
    Channel& channel = ChannelManager::GetChannel(channelName);
    const std::string& targetUser = parameters[1];
    if (channel.IsUserExist(targetUser) == false)
    {
        messageToReply.append(ERR_USERNOTINCHANNEL);
        messageToReply.append(" ");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(targetUser);
        messageToReply.append(" ");
        messageToReply.append(channelName);
        messageToReply.append(" ");
        messageToReply.append(":They aren't on that channel");
        messageToReply.append(CRLF);
        network.PushToSendBuffer(socket, messageToReply);
        return;
    }
    // ERR_NOTONCHANNEL
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
        return;
    }
    // 채널의 모든 유저에게 전송 및 채널에서 유저 삭제
    messageToReply.append(":");
    messageToReply.append(user.GetNickname());
    messageToReply.append("!");
    messageToReply.append(user.GetUsername());
    messageToReply.append("@");
    messageToReply.append(user.GetHostname());
    messageToReply.append(" ");
    messageToReply.append(command);
    messageToReply.append(" ");
    messageToReply.append(channelName);
    messageToReply.append(" ");
    messageToReply.append(targetUser);
    messageToReply.append(" ");
    if (trailing.empty() == false)
    {
        messageToReply.append(":");
        messageToReply.append(trailing);
    }
    messageToReply.append(CRLF);
    std::map<std::string, User>::const_iterator userInChannel = channel.GetUsers().begin();
    while (userInChannel != channel.GetUsers().end())
    {
        network.PushToSendBuffer(userInChannel->second.GetSocket(), messageToReply);
        userInChannel++;
    }
    channel.DeleteUser(targetUser);
    channel.DeleteOperator(targetUser);
    channel.DeleteInvitedUser(targetUser);
    ChannelManager::CheckIsEmptyChannelAndDelete(channel);
}

void IRC::PRIVMSG(const int32 IN socket,
                  const std::string& IN command,
                  const std::vector<std::string>& IN parameters,
                  const std::string& IN trailing,
                  const std::string& IN password,
                  Network& IN OUT network)
{
    if (UserManager::GetUser(socket).IsRegistered() == false)
    {
        return;
    }
    static_cast<void>(password);
    const User& user = UserManager::GetUser(socket);
    std::string messageToReply("");
    // ERR_NORECIPIENT
    if (parameters.size() == 0)
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
    const std::string& target = parameters[0];
    bool IsChannel = false;
    if (target[0] == '&' || target[0] == '#')
    {
        IsChannel = true;
    }
    // ERR_NOSUCHNICK
    if ((IsChannel && ChannelManager::IsChannelExist(target) == false)
        || (IsChannel == false && UserManager::IsUserExist(target) == false))
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
    // target이 채널인 경우
    if (IsChannel)
    {
        messageToReply.append(":");
        messageToReply.append(user.GetNickname());
        messageToReply.append(" ");
        messageToReply.append(command);
        messageToReply.append(" ");
        messageToReply.append(target);
        messageToReply.append(" :");
        messageToReply.append(trailing);
        messageToReply.append(CRLF);
        const Channel& channel = ChannelManager::GetChannel(target);
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
        messageToReply.clear();
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
        messageToReply.clear();
    }
}

void IRC::PING(const int32 IN socket,
               const std::string& IN command,
               const std::vector<std::string>& IN parameters,
               const std::string& IN trailing,
               const std::string& IN password,
               Network& IN OUT network)
{
    static_cast<void>(command);
    static_cast<void>(trailing);
    static_cast<void>(password);
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
    static_cast<void>(command);
    static_cast<void>(trailing);
    static_cast<void>(password);
    const std::string& token = parameters[0];
    User& user = UserManager::GetUser(socket);
    if (user.IsRegistered() == false && token == network.GetIPString(network.GetServerSocket()))
    {
        user.SetRegistered();
    }
    sendWelcomeMessage(socket, network);
}

void IRC::sendWelcomeMessage(const int32 IN socket, Network& IN OUT network)
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