/**
 * @file User.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief IRC User 클래스
 * @version 0.1
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "common.hpp"

using namespace gdf;

namespace grc
{

class User
{
public:
    User();
    const User& operator=(const User& User);
    virtual ~User();

    bool IsRegisterd() const;
    bool IsChannelOperator() const;
private:
    User(const User& User); // = delete

private:
    bool mbIsRegisterd;
    bool mbIsChannelOperator;
    std::string mNickname;
    std::string mUsername;
    std::string mHostname;
    std::string mServername;
    std::string mRealname;
    std::string mCurrentChannel;
};

}
