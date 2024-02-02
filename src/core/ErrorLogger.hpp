/**
 * @file ErrorLogger.hpp
 * @author Jeekun Park (jeekunp@naver.com)
 * @brief 에러 로거 클래스 헤더
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <fstream>
#include <ctime>
#include <sstream>
#include "common.hpp"


class ErrorLogger
{
public:
    static ErrorLogger& getInstance();
    void log(const std::string& message);

private:
    std::ofstream mLogFile;

    ErrorLogger();
    ErrorLogger(const ErrorLogger&);
    ErrorLogger& operator=(const ErrorLogger&);
    ~ErrorLogger(); 
};