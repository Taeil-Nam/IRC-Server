/**
 * @file ChannelManager.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief Channel 관리용 정적 클래스
 * @version 0.1
 * @date 2024-04-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "common.hpp"
#include "Channel.hpp"

namespace grc
{

class ChannelManager
{
public:
    static void AddChannel(const std::string& IN channelName);
    static void DeleteChannel(const std::string& IN channelName);

    static bool IsChannelExist(const std::string& IN channelName);

    static Channel& GetChannel(const std::string IN channelName);
    static const std::map<std::string, Channel>& GetChannels();
private:
    ChannelManager(); // = delete

private:
    static std::map<std::string, Channel> mChannels;
};

}
