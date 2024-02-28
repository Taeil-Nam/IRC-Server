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

// standard (표준 헤더)
#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <map>
#include <deque>


// custom (사용자 정의 헤더)
#include "types.hpp"
#include "utils/isInt.hpp"
#include "utils/LogManager.hpp"
#include "utils/AssertManager.hpp"

// 공용 매크로
#define SUCCESS true
#define FAILURE false
#define ERROR -1

#define IRC_MESSAGE_SIZE 512
#define BufferSize 1024