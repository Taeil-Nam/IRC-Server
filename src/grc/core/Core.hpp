/**
 * @file Core.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief 메인 함수에서 객체 생성되는 코어 클래스
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "common.hpp"

namespace grc
{

class Core
{
public:
    Core(const int port, const std::string& password);
    ~Core();

    void Run();
private:
    Core();
    Core(const Core& core);
    const Core& operator=(const Core& core);

private:
    const int mPort;
    const std::string mPassword;
};

}
