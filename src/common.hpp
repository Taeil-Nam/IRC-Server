/**
 * @file common.hpp
 * @author tnam (tnam@student.42seoul.kr)
 * @brief 자주 사용되는 헤더 파일을 모아놓은 헤더 파일
 * @version 0.1
 * @date 2024-02-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define GAMERC_VERSION ("GameRC v1.0.0")

// standard (표준 헤더)
#include <iostream>
#include <string>
#include <ctime>
#include <set>

// BSD-GDF 라이브러리
#include <BSD-GDF/Assert.hpp>
#include <BSD-GDF/Logger.hpp>
#include <BSD-GDF/Display.hpp>
#include <BSD-GDF/Event.hpp>
#include <BSD-GDF/Network.hpp>

// custom (사용자 정의 헤더)
#include "utils/isInt.hpp"
#include "utils/split.hpp"


// 공용 매크로
#define IRC_MESSAGE_SIZE 512

