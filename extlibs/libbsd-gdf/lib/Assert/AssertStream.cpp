#include "../../include/BSD-GDF/Assert/AssertStream.hpp"

namespace gdf
{

AssertStream::AssertStream(bool condition, const char* expressionString,
                const char* functionName, const char* fileName,
                const int lineNumber)
: bCondition(condition)
, mExpressionString(expressionString)
, mFunctionName(functionName)
, mFileName(fileName)
, mLineNumber(lineNumber)
{}

AssertStream::~AssertStream()
{
    if (!bCondition)
    {
        std::cerr << "\n[Assertion failure]\n" 
                    << " - Assert:\t(" << mExpressionString << ")\n"
                    << (mStream.str().size() > 0 ? (" - Message:\t\"" + mStream.str() + "\"\n") : "")
                    << " - File:\t" << mFileName << ':' << mLineNumber <<  '\n'
                    << " - Function:\t" << mFunctionName << "\n\n"
                    << std::flush;
        std::abort();
    }
}

}

