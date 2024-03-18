#ifndef GDF_COMMON_HPP
#define GDF_COMMON_HPP

#if !defined(__amd64__) && !defined(__aarch64__)
#error "This library is intended to be compiled only on a 64-bit system."
#endif

#include "./Config/types.hpp"

#define IN
#define OUT

#define SUCCESS true
#define FAILURE false
#define ERROR -1


#endif