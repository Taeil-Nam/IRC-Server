/**
 * @file IAnimation.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief IAnimation 인터페이스에 대한 정의
 * @version 0.1
 * @date 2024-04-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "BSD-GDF/Display/DisplayConsole.hpp"
#include "common.hpp"

namespace grc
{

/**
 * @brief ASCII 애니메이션 출력을 위한 인터페이스
 * 
 * IAnimation 인터페이스는 애니페이션 객체가 구현해야하는 핵심 기능을 선언한다.
 * 모든 애니메이션 구현체는 이 인터페이스를 상속받아, PrintNextFrame 함수를 구현해야한다.
 */
interface IAnimation
{
    /**
     * @brief 소멸자. 파생된 클래스가 올바르게 소멸될 수 있도록 한다.
     */
    virtual ~IAnimation() {}

    /**
     * @brief 다음 애니메이션 프레임을 디스플레이에 출력한다.
     * 
     * 이 메소드는 파생된 애니메이션 클래스에서 구현되어야 한다.
     * 디스플레이 장치에 다음 프레임을 그리는 로직이 포함되어야 한다.
     * 프레임은 각각의 애니메이션 단계를 나타내며, 연속적인 호출을 통해 애니메이션 효과를 생성한다.
     *
     * @param monitor gdf::DisplayConsole 타입의 디스플레이 장치 참조
     */
    virtual void PrintNextFrame(gdf::DisplayConsole& monitor) = 0;
};

}
