/**
 * @file ChannelManager.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief ChannelManager 클래스 정의 헤더 파일.
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

/**
 * @class ChannelManager
 * @brief Channel 객체들을 관리하는 정적 클래스.
 * 
 */
class ChannelManager
{
public:
    /**
     * @brief Channel 객체가 존재하는지 확인하는 함수.
     * 
     * @param channelName 찾을 Channel 객체의 이름.
     * @return true
     * @return false
     */
    static bool IsChannelExist(const std::string& IN channelName);

    /**
     * @brief 특정 Channel 객체를 반환하는 함수.
     * 
     * 없는 채널에 대해서 함수를 호출하는 경우, Assertion 발생.
     * 
     * @param channelName 찾을 Channel 객체의 이름.
     * @return Channel& : Channel의 객체를 반환.
     */
    static Channel& GetChannel(const std::string IN channelName);
    /**
     * @brief 현재 모든 Channel 객체의 목록을 반환하는 함수.
     * 
     * @return std::map<std::string, Channel>& : 멤버 변수 sStaticChannels 반환.
     */
    static std::map<std::string, Channel>& GetChannels();

    /**
     * @brief Channel 객체를 추가하는 함수.
     * 
     * @param channelName 추가할 Channel 객체의 이름.
     */
    static void AddChannel(const std::string& IN channelName);
    /**
     * @brief Channel 객체를 삭제하는 함수.
     * 
     * @param channelName 삭제할 Channel 객체의 이름.
     */
    static void DeleteChannel(const std::string& IN channelName);
    /**
     * @brief 특정 채널에 존재하는 유저를 삭제하는 함수.
     * 
     * @param user 삭제할 User 객체.
     * @param channelName 유저가 삭제될 Channel 객체의 이름.
     */
    static void DeleteUserFromChannel(const User& IN user, const std::string& channelName);
    /**
     * @brief 모든 채널에서 유저를 삭제하는 함수.
     * 
     * @param user 삭제할 User 객체.
     */
    static void DeleteUserFromAllChannels(const User& IN user);
    /**
     * @brief 채널이 비었는지 확인 후, 채널을 삭제하는 함수.
     * 
     * 채널이 비어있지 않은 경우 삭제하지 않는다.
     * 
     * @param channel 검사할 Channel 객체의 이름.
     */
    static void CheckIsEmptyChannelAndDelete(const Channel& IN channel);
private:
    /**
     * @brief ChannelManager 객체의 기본 생성자. (사용되지 않음)
     * 
     */
    ChannelManager(); // = delete


private:
    /**
     * @brief 현재 IRC에 생성된 모든 Channel 객체의 목록.
     * 
     */
    static std::map<std::string, Channel> sStaticChannels;
};

}
