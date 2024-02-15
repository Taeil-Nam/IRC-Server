#include <sstream>
#include "toString.hpp"
#include <iostream> //
std::string toString(int value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(long value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(long long value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(unsigned value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(unsigned long value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(unsigned long long value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(float value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(double value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}

std::string toString(long double value)
{
	std::stringstream ss;

    ss << value;
    return ss.str();
}
