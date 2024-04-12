/**
 * @file Earth.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief IAnimation 인터페이스를 구현하는 Earth 클래스를 정의한다.
 * @version 0.1
 * @date 2024-04-12
 * 
 * @copyright Copyright (c) 2024
 */

#pragma once

#include "common.hpp"
#include <vector>

#include "grc/animation/IAnimation.hpp"

namespace grc
{

/**
 * @class Earth
 * @brief 지구의 애니메이션을 출력하는 클래스이다.
 * 
 * Earth 클래스는 IAnimation 인터페이스를 구현하여 지구의 애니메이션 표현을 제공한다.
 * 여러 프레임의 애니메이션을 저장하고 순차적으로 표시한다.
 */
class Earth : public IAnimation
{
public:

    /**
     * @brief Earth 클래스의 기본 생성자
     */
    Earth();

    /**
     * @brief Earth 클래스의 복사 생성자
     * @param copy 복사할 Earth 객체의 참조
     */
    Earth(const Earth& copy);

    /**
     * @brief Earth 클래스의 복사 대입 연산자
     * @param copy 복사할 Earth 객체의 참조
     * @return Earth& 업데이트된 객체의 참조를 반환
     */
    Earth& operator=(const Earth& copy);

    /**
     * @brief Earth 클래스의 소멸자
     */
    virtual ~Earth();

    /**
     * @brief 인자로 받은 디스플레이 콘솔에 다음 애니메이션 프레임을 출력한다.
     * 
     * 연속적인 호출을 통해 애니메이션 효과를 생성한다.
     *
     * @param monitor 프레임이 출력될 gdf::DispalyConsole 객체의 참조
     */
    virtual void PrintNextFrame(gdf::DisplayConsole& monitor);
private:
    enum { FRAMES_SIZE = 30}; ///< 애니메이션 시퀀스의 전체 프레임 수
    uint64 mFramesIndex; ///< 프레임 시퀀스에서의 현재 프레임 인덱스
    std::vector<std::string> mFrames; ///< 각 애니메이션 프레임을 저장하는 배열
};

} // namespace grc