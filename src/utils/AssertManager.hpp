/* Private API */

#ifndef ASSERT_MANAGER_HPP
#define ASSERT_MANAGER_HPP

#define ASSERT_MANAGER_VERSION "1.0.0"

#include <sstream>
#include <iostream>

#if !defined(__clang__) && !defined(__GNUC__)
    #define __PRETTY_FUNCTION__ __FUNCTION__
#endif

class AssertManager
{
public:
    static void Assert(bool condition, const char* expressionString, const std::string& message,
                const char* functionName, const char* fileName, const int lineNumber)
    {
        if (condition)
            return ;
        else
        {
            std::cerr << "\n[Assertion failure]\n" 
                      << " - Assert:\t(" << expressionString << ")\n"
                      << (message.length() > 0 ? (" - Message:\t\"" + message + "\"\n") : "")
                      << " - File:\t" << fileName << ':' << lineNumber <<  '\n'
                      << " - Function:\t" << functionName << "\n\n"
                      << std::flush;
            std::abort();
        }
    }

    class AssertStream
    {
    public:
        AssertStream(bool condition, const char* expressionString,
                     const char* functionName, const char* fileName,
                     const int lineNumber)
        : bCondition(condition)
        , mExpressionString(expressionString)
        , mFunctionName(functionName)
        , mFileName(fileName)
        , mLineNumber(lineNumber)
        {}
        ~AssertStream()
        {
            if (!bCondition)
                AssertManager::Assert(bCondition, mExpressionString,
                                      mStream.str(), mFunctionName,
                                      mFileName, mLineNumber);
        }
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
};

#endif

/* Public API */

#ifdef ASSERT
#undef ASSERT
#endif

#if !(defined(NDEBUG) || defined(_NDEBUG))
#define ASSERT(e) AssertManager::AssertStream(e, #e, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define ASSERT(e) if(false) std::cerr
#endif


