/* #pragma once

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "common.hpp"

namespace grc
{

class ConsoleWindow
{
public:
    enum eANSIColor
    {
        Default = 0,
        Cyan,
        Red,
        Green,
        BrightBlue,
        WhiteCharBlueBG,
        WhiteCharRedBG
    };
    ConsoleWindow();
    ~ConsoleWindow();
    ConsoleWindow(const ConsoleWindow& copy);
    ConsoleWindow& operator=(const ConsoleWindow& copy);
    void PushBackCommandLine(const char c);
    std::string In();
    void Out(const std::string& str, const eANSIColor color = Default);
    bool IsEOF() const;
    
    void RefreshConsole();
    void SetHeaderColor(const eANSIColor color);
    void SetFooterColor(const eANSIColor color);
    void SetHeader(const std::string& str);
    void SetFooter(const std::string& str);
    void SetTimestamp(const bool enable);
private:
    struct OutputFormat
    {
        std::string TimeStamp;
        std::string String;
        eANSIColor Color;
    };
    void setTerminalMode(const bool enable);
    std::string getCurrentTimeString() const;
    void updateConsoleSize();
    void locateCursorToLine(const int32 line) const;
    uint64 strlenMultiByte(const std::string& str) const;
    void printClear() const;
    void printErrorScreenFull() const;
    void printHeader() const;
    void printOutput() const;
    void printFooter() const;
    void printCommandLine() const;
    void printFlush() const;

private:
    std::string mHeaderColor;
    std::string mHeader;
    std::string mFooterColor;
    std::string mFooter;
    std::map<eANSIColor, std::string> mANSIColors;
    std::string mCommandLineBuffer;
    std::deque<std::string> mInputBuffer;
    std::deque<OutputFormat> mOutputBuffer;
    int16 mConsoleWidth;
    int16 mConsoleHeight;
    bool bIsTimestampEnabled;
    uint64 mOutputBufferCapacity;
};

}
 */

#pragma once

#include <ctime>

#include "common.hpp"

namespace grc
{

class Display
{
public:
    enum eColor
    {
        Default = 0,
        Cyan,
        Red,
        Green,
        BrightBlue,
        WhiteCharBlueBG,
        WhiteCharRedBG
    };
    struct Content
    {
        std::string TimeStamp;
        std::string String;
        eColor Color;
    };
    Display();
    virtual ~Display();
    Display(const Display& copy);
    Display& operator=(const Display& IN copy);
    void Clear();
    void PushContent(const std::string& IN str, Display::eColor IN color);
    const std::deque<Display::Content>& GetContentBuffer() const;
    const std::string& GetHeader() const;
    const std::string& GetFooter() const;
    void SetHeader(const std::string& IN str);
    void SetFooter(const std::string& IN str);
    void SetContentBufferCapacity(const uint64 IN capacity);

private:
    void getCurrentTimeString(std::string& OUT str);

private:
    std::string mHeader;
    std::string mFooter;
    uint64 mContentBufferCapacity;
    std::deque<Content> mContentBuffer;
    
};

}
