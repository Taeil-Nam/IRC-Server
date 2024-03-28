#include "split.hpp"

std::vector<std::string> split(std::string input, std::string delimiter)
{
    std::vector<std::string> result;
    std::size_t pos = 0;
    std::string token = "";
    while ((pos = input.find(delimiter)) != std::string::npos)
    {
        token = input.substr(0, pos);
        if (token != "")
        {
            result.push_back(token);
        }
        input.erase(0, pos + delimiter.size());
    }
    result.push_back(input);
    return result;
}
