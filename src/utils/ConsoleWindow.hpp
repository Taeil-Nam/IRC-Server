#pragma once

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
    
    void RefreshScreen();
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
