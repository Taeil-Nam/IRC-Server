#pragma once

#include "./Assert/AssertStream.hpp"

#if !defined (__clang__) && !defined (__GNUC__)
#define __PRETTY_FUNCTION__ (__FUNCTION__)
#endif

#ifdef ASSERT
#undef ASSERT
#endif

/* Public API */
#if !(defined (NDBUG) || defined (_NDBUF))
#define ASSERT(e) gdf::AssertStream(e, #e, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define ASSERT(e) if(false) std::cerr
#endif
