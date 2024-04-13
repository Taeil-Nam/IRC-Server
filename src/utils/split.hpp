/**
 * @file split.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief 이 파일은 split 함수를 정의한다.
 * @version 0.1
 * @date 2024-04-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>
#include <vector>

/**
 * @brief 특정 구분자로 나뉜 문자열들을 std::vector로 반환한다.
 * 
 * - Time complexity : O(n^2).
 * 
 * @param input 대상 문자열.
 * @param delimiter 구분자.
 * @return std::vector<std::string> : 구분자로 나뉘어진 문자열의 배열.
 */
std::vector<std::string> split(std::string input, std::string delimiter);
