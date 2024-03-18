#pragma once

#include <sstream>
#include <iostream>

namespace gdf
{

class AssertStream
{
public:
    AssertStream(bool condition, const char* expressionString,
                 const char* functionName, const char* fileName,
                 const int lineNumber);
    ~AssertStream();
    template<typename T>
    AssertStream& operator<<(const T& message)
    {
        mStream << message;
        return *this;
    }

private:
    bool bCondition;
    const char* mExpressionString;
    const char* mFunctionName;
    const char* mFileName;
    const int mLineNumber;
    std::ostringstream mStream;
};

}
