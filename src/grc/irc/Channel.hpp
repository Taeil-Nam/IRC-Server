/**
 * @file Channel.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief Channel 클래스 정의 헤더 파일.
 * @version 0.1
 * @date 2024-03-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../../common.hpp"
#include "User.hpp"

namespace grc
{

/**
 * @class Channel
 * @brief IRC에서 생성되는 Channel 객체를 정의하는 클래스.
 */
class Channel
{
public:
    /**
     * @brief 채널에 입장 가능한 최대 유저 수의 제한이 없음을 나타낸다.
     */
    enum { UNLIMIT = 0 };

public:
    /**
     * @brief Channel 객체의 기본 생성자.
     * 
     * 객체가 생성될 때, 모든 멤버 변수들을 초기화한다.
     */
    Channel();
    /**
     * @brief Channel 객체의 소멸자.
     */
    virtual ~Channel();

    /**
     * @brief 채널에 유저가 존재하는지 확인하는 함수.
     * 
     * @param nickname target nickname.
     * @return true
     * @return false
     */
    bool IsUserExist(const std::string& IN nickname) const;
    /**
     * @brief 유저가 채널의 operator인지 확인하는 함수.
     * 
     * @param nickname target nickname.
     * @return true
     * @return false
     */
    bool IsOperator(const std::string& IN nickname) const;
    /**
     * @brief 유저가 채널에 초대되었는지 확인하는 함수.
     * 
     * @param nickname target nickname.
     * @return true
     * @return false
     */
    bool IsInvited(const std::string& IN nickname) const;
    /**
     * @brief 채널의 topic을 operator만 설정할 수 있는지 확인하는 함수.
     * 
     * @return true 
     * @return false 
     */
    bool IsProtectedTopic() const;
    /**
     * @brief 채널이 초대 전용으로 설정되었는지 확인하는 함수.
     * 
     * @return true 
     * @return false 
     */
    bool IsInviteOnly() const;
    /**
     * @brief 채널에 key가 설정되었는지 확인하는 함수.
     * 
     * @return true 
     * @return false 
     */
    bool IsKeyRequired() const;
    /**
     * @brief 채널에 최대 입장 유저 수 제한이 설정되었는지 확인하는 함수.
     * 
     * @return true 
     * @return false 
     */
    bool IsLimitedMaxUserCount() const;
    /**
     * @brief 채널에 유저가 없는지 확인하는 함수.
     * 
     * @return true 
     * @return false 
     */
    bool IsChannelEmpty() const;

    /**
     * @brief Channel 객체의 name을 반환하는 함수.
     * 
     * @return const std::string& : Channel 객체의 name.
     */
    const std::string& GetName() const;
    /**
     * @brief Channel 객체의 topic을 반환하는 함수.
     * 
     * @return const std::string& : Channel 객체의 topic.
     */
    const std::string& GetTopic() const;
    /**
     * @brief Channel 객체의 max user count을 반환하는 함수.
     * 
     * @return uint32 : Channel 객체의 max user count.
     */
    uint32 GetMaxUserCount() const;
    /**
     * @brief Channel 객체의 key를 반환하는 함수.
     * 
     * @return const std::string& : Channel 객체의 key.
     */
    const std::string& GetKey() const;
    /**
     * @brief 현재 채널에 존재하는 유저의 수를 반환하는 함수.
     * 
     * @return uint32 : Channel 객체에 존재하는 유저의 수.
     */
    uint32 GetCurrentUserCount() const;
    /**
     * @brief 현재 채널에 존재하는 유저 목록을 반환하는 함수.
     * 
     * @return const std::map<std::string, User>& : 멤버 변수 mUsers 반환.
     */
    const std::map<std::string, User>& GetUsers() const;
    /**
     * @brief 현재 채널에 존재하는 operator 목록을 반환하는 함수.
     * 
     * @return const std::map<std::string, User>& : 멤버 변수 mOperators 반환.
     */
    const std::map<std::string, User>& GetOperators() const;
    /**
     * @brief 현재 채널에 존재하는 유저들의 nickname 목록을 반환하는 함수.
     * 
     * @return std::string : 공백(' ')으로 분리된 채널에 존재하는 유저 nickname 목록.
     */
    std::string GetAllUsersNickname() const;
    /**
     * @brief 현재 채널에 설정된 mode를 반환하는 함수.
     * 
     * @return std::string : 채널에 설정된 mode.
     */
    std::string GetModeString() const;
    /**
     * @brief 현재 채널에 설정된 mode에 대한 인자들을 반환하는 함수.
     * 
     * @return std::string : 채널에 설정된 mode에 대한 인자.
     */
    std::string GetModeArgument() const;

    /**
     * @brief Channel 객체의 name을 설정하는 함수.
     * 
     * @param channelName target channelName.
     */
    void SetName(const std::string& IN channelName);
    /**
     * @brief Channel 객체의 topic을 설정하는 함수.
     * 
     * @param topic target topic.
     */
    void SetTopic(const std::string& IN topic);
    /**
     * @brief Channel 객체의 max user count를 설정하는 함수.
     * 
     * @param maxUserCount target maxUserCount.
     */
    void SetMaxUserCount(uint32 IN maxUserCount);
    /**
     * @brief Channel 객체의 key를 설정하는 함수.
     * 
     * @param key target key.
     */
    void SetKey(const std::string& IN key);
    /**
     * @brief 채널의 topic을 operator만 설정할 수 있도록 하는 함수.
     * 
     * 멤버 변수 mbIsProtectedTopic을 true로 설정한다.
     */
    void SetProtectedTopic();
    /**
     * @brief 채널을 초대 전용으로 설정하는 함수.
     * 
     * 멤버 변수 mbIsInviteOnly를 true로 설정한다.
     */
    void SetInviteOnly();
    /**
     * @brief 채널에 입장 가능한 최대 유저 수 제한을 설정하는 함수.
     * 
     * 멤버 변수 mbIsLimitedMaxUserCount를 true로 설정한다.
     */
    void SetLimitedMaxUserCount();
    /**
     * @brief 채널의 key를 설정하는 함수.
     * 
     * 멤버 변수 mbIsKeyRequired를 true로 설정한다.
     */
    void SetKeyRequired();

    /**
     * @brief 채널의 topic을 모두가 설정할 수 있도록 하는 함수.
     * 
     * 멤버 변수 mbIsProtectedTopic을 false로 설정한다.
     */
    void UnsetProtectedTopic();
    /**
     * @brief 초대 전용 설정을 해제하는 함수.
     * 
     * 멤버 변수 mbIsInviteOnly를 false로 설정한다.
     */
    void UnsetInviteOnly();
    /**
     * @brief 채널에 입장 가능한 최대 유저 수 제한을 해제하는 함수.
     * 
     * 멤버 변수 mbIsLimitedMaxUserCount를 false로 설정한다.
     */
    void UnsetLimitedMaxUserCount();
    /**
     * @brief 채널의 key를 해제하는 함수.
     * 
     * 멤버 변수 mbIsKeyRequired를 false로 설정한다.
     */
    void UnsetKeyRequired();

    /**
     * @brief 채널에 유저를 추가하는 함수.
     * 
     * @param nickname 추가할 유저의 nickname.
     * @param user 추가할 User 객체.
     */
    void AddUser(const std::string& IN nickname, const User& IN user);
    /**
     * @brief 채널에 operator를 추가하는 함수.
     * 
     * @param nickname 추가할 operator의 nickname.
     * @param user 추가할 User 객체.
     */
    void AddOperator(const std::string& IN nickname, const User& IN user);
    /**
     * @brief 채널에 초대 받은 유저를 추가하는 함수.
     * 
     * @param nickname 초대 받은 유저의 nickname.
     * @param user 추가할 User 객체.
     */
    void AddInvitedUser(const std::string& IN nickname, const User& IN user);
    /**
     * @brief 채널의 topic을 삭제하는 함수.
     * 
     */
    void DeleteTopic();
    /**
     * @brief 채널의 유저를 삭제하는 함수.
     * 
     * @param nickname 삭제할 유저의 nickname.
     */
    void DeleteUser(const std::string& IN nickname);
    /**
     * @brief 채널의 operator를 삭제하는 함수.
     * 
     * @param nickname 삭제할 operator의 nickname.
     */
    void DeleteOperator(const std::string& IN nickname);
    /**
     * @brief 채널에 초대 받은 유저를 삭제하는 함수.
     * 
     * @param nickname 삭제할 유저의 nickname.
     */
    void DeleteInvitedUser(const std::string& IN nickname);
private:
    /**
     * @brief Channel 객체의 복사 생성자. (사용되지 않음)
     * 
     * @param channel 복사할 Channel 객체.
     */
    Channel(const Channel& IN channel); // = delete
    /**
     * @brief Channel 객체의 복사 대입 연산자. (사용되지 않음)
     * 
     * @param channel 복사할 Channel 객체.
     * @return const Channel& : 복사된 Channel 객체.
     */
    const Channel& operator=(const Channel& IN channel); // = delete

private:
    /**
     * @brief Channel 객체의 이름을 저장하는 멤버 변수.
     */
    std::string mName;
    /**
     * @brief Channel 객체의 topic을 저장하는 멤버 변수.
     */
    std::string mTopic;
    /**
     * @brief Channel 객체의 max user count을 저장하는 멤버 변수.
     */
    uint32 mMaxUserCount;
    /**
     * @brief Channel 객체의 key를 저장하는 멤버 변수.
     */
    std::string mKey;
    /**
     * @brief 채널에 입장한 유저의 목록을 저장하는 멤버 변수.
     */
    std::map<std::string, User> mUsers;
    /**
     * @brief 채널에 입장한 operator의 목록을 저장하는 멤버 변수.
     */
    std::map<std::string, User> mOperators;
    /**
     * @brief 채널에 초대된 유저의 목록을 저장하는 멤버 변수.
     */
    std::map<std::string, User> mInvitedUsers;
    /**
     * @brief 채널의 topic 설정에 operator 권한이 필요한 상태인지를 저장하는 멤버 변수.
     */
    bool mbIsProtectedTopic;
    /**
     * @brief 채널이 초대 전용 상태인지 저장하는 멤버 변수.
     */
    bool mbIsInviteOnly;
    /**
     * @brief 채널에 입장 가능한 최대 유저 수 제한이 설정된 상태인지를 저장하는 멤버 변수.
     */
    bool mbIsLimitedMaxUserCount;
    /**
     * @brief 채널에 key가 설정된 상태인지 저장하는 멤버 변수.
     */
    bool mbIsKeyRequired;
};

}
