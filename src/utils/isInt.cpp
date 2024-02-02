#include "utils/isInt.hpp"

bool isInt(const std::string& literal)
{
	char* endPtr = NULL;
	const char* cStr = literal.c_str();
	long longValue = std::strtol(cStr, &endPtr, 10);
	if (literal.length() != 0
		&& *endPtr == '\0'
		&& (static_cast<long>(std::numeric_limits<int>::min()) <= longValue 
			&& longValue <= static_cast<long>(std::numeric_limits<int>::max())))
		return true;
	else
		return false;
}
