/**
 * @file split.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief 문자열을 특정 문자열을 기준으로 구분해서 반환하는 함수 O(n^2)
 * @version 0.1
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(std::string input, std::string delimiter);
