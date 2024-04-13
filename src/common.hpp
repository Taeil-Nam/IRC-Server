/**
 * @file Channel.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief 이 파일은 프로그램에서 공통적으로 사용되는 헤더 파일과 
 * 매크로를 정의한다.
 * 
 * @version 0.1
 * @date 2024-04-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

/**
 * @brief
 * IRC 서버의 버전을 나타내는 매크로.
 */
#define GAMERC_VERSION ("GameRC v1.0.0")

/**
 * @brief
 * interface 구분을 위한 매크로.
 */
#define interface struct

// standard (표준 헤더)
#include <iostream>
#include <string>
#include <map>

// BSD-GDF 라이브러리
#include <BSD-GDF/Assert.hpp>
#include <BSD-GDF/Logger.hpp>
#include <BSD-GDF/Display.hpp>
#include <BSD-GDF/Event.hpp>
#include <BSD-GDF/Network.hpp>

// custom (사용자 정의 헤더)
#include "utils/isInt.hpp"
#include "utils/split.hpp"
