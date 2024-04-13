/**
 * @file isInt.hpp
 * @author tnam (tnam@student.42seoul.kr)
 * @brief 문자열로 되어있는 숫자가 int인지 판단하는 함수
 * @version 0.1
 * @date 2024-02-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>

/**
 * @brief 문자열이 int의 리터럴인지 판별하는 함수
 * 
 * 이 함수는 주어진 문자열이 int의 리터럴이면서 10진 정수 형식에 맞는지 검사한다.
 * 
 * @param literal 검사할 문자열
 * @return true 성공시
 * @return false 실패시
 */
bool isInt(const std::string& literal);
