#include "ChannelManager.hpp"
#include "BSD-GDF/Assert.hpp"

namespace grc
{

std::map<std::string, Channel> ChannelManager::mChannels;

void ChannelManager::AddChannel(const std::string& IN channelName)
{
    if (mChannels.count(channelName) == 0)
    {
        mChannels[channelName];
        mChannels[channelName].SetName(channelName);
    }
}

void ChannelManager::DeleteChannel(const std::string& IN channelName)
{
    if (mChannels.count(channelName) > 0)
    {
        mChannels.erase(channelName);
    }
}

bool ChannelManager::IsChannelExist(const std::string& IN channelName)
{
    if (mChannels.count(channelName) > 0)
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
    ASSERT(mChannels.count(channelName) > 0)
        << "Channel must exist";
    return mChannels[channelName];
}

const std::map<std::string, Channel>& ChannelManager::GetChannels()
{
    return mChannels;
}

}