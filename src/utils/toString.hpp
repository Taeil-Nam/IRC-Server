/**
 * @file toString.hpp
 * @author tnam (tnam@student.42seoul.kr)
 * @brief 숫자 자료형을 string으로 변환하는 함수
 * @version 0.1
 * @date 2024-02-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>

std::string toString(int value);
std::string toString(long value);
std::string toString(long long value);
std::string toString(unsigned value);
std::string toString(unsigned long value);
std::string toString(unsigned long long value);
std::string toString(float value);
std::string toString(double value);
std::string toString(long double value);
