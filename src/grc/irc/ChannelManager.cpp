#include "ChannelManager.hpp"
#include "BSD-GDF/Assert.hpp"

namespace grc
{

std::map<std::string, Channel> ChannelManager::sStaticChannels;

bool ChannelManager::IsChannelExist(const std::string& IN channelName)
{
    if (sStaticChannels.count(channelName) > 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

Channel& ChannelManager::GetChannel(const std::string IN channelName)
{
    ASSERT(sStaticChannels.count(channelName) > 0)
        << "Channel must exist";
    return sStaticChannels[channelName];
}

std::map<std::string, Channel>& ChannelManager::GetChannels()
{
    return sStaticChannels;
}

void ChannelManager::AddChannel(const std::string& IN channelName)
{
    if (sStaticChannels.count(channelName) == 0)
    {
        sStaticChannels[channelName];
        sStaticChannels[channelName].SetName(channelName);
        LOG(LogLevel::Informational) << "Create new channel " << "[" << channelName << "]";
    }
}

void ChannelManager::DeleteChannel(const std::string& IN channelName)
{
    if (sStaticChannels.count(channelName) > 0)
    {
        sStaticChannels.erase(channelName);
        LOG(LogLevel::Informational) << "Delete channel " << "[" << channelName << "]";
    }
}

void ChannelManager::DeleteUserFromChannel(const User& IN user, const std::string& channelName)
{
    if (sStaticChannels.count(channelName) > 0)
    {
        sStaticChannels[channelName].DeleteUser(user.GetNickname());
    }
}

void ChannelManager::DeleteUserFromAllChannels(const User& IN user)
{
    std::vector<std::string> deleteChannelList;
    std::map<std::string, Channel>::iterator it = sStaticChannels.begin();
    while (it != sStaticChannels.end())
    {
        Channel& channel = it->second;
        channel.DeleteUser(user.GetNickname());
        deleteChannelList.push_back(channel.GetName());
        it++;
    }
    for (std::size_t i = 0; i < deleteChannelList.size(); i++)
    {
        CheckIsEmptyChannelAndDelete(ChannelManager::GetChannel(deleteChannelList[i]));
    }
}

void ChannelManager::CheckIsEmptyChannelAndDelete(const Channel& IN channel)
{
    if (channel.IsChannelEmpty())
    {
        DeleteChannel(channel.GetName());
    }
}

}