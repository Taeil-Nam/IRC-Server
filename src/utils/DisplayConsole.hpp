#pragma once

#include <deque>
#include <map>
#include <sstream>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.hpp"

#include "Display.hpp"

namespace grc
{

class DisplayConsole
{
public:
    DisplayConsole(const std::string& IN header,
                   const std::string& IN footer,
                   const Display::eColor IN headerColor,
                   const Display::eColor IN footerColor);
    virtual ~DisplayConsole();
    void PushCharPrompt(const char IN ch);
    bool pollPromptQueue(std::string& OUT prompt);
    void PushContent(const std::string& IN content, Display::eColor IN color);
    void ClearContent();



    void SetHeader(const std::string& IN str);
    void SetFooter(const std::string& IN str);
    void SetHeaderColor(const Display::eColor IN color);
    void SetFooterColor(const Display::eColor IN color);
    void SetTimestamp(const bool IN enable);

    
    void NonBlockWrite();
private: // function
    void setTerminalMode(const bool IN enable);
    void updateConsoleSize();
    std::string cursorToLine(const int32 line) const;
    uint64 strlenMultiByte(const std::string& str) const;
    
    void renderScreenString(std::string& OUT screenBuffer);
    void appendHeader(std::string& OUT screenBuffer);
    void appendContent(std::string& OUT screenBuffer);
    void appendFooter(std::string& OUT screenBuffer);
    void appendPrompt(std::string& OUT screenBuffer);
private: // variables
    static uint64 sStaticInstanceCount;
    static struct termios sStaticOldTerminal;
    
    Display mDisplay;
    // general
    std::map<Display::eColor, std::string> mANSIColors;
    std::string mHeaderColor;
    std::string mFooterColor;
    bool bIsTimestampEnabled;

    // input
    std::string mPromptBuffer;
    std::deque<std::string> mPromptQueue;

    // attr
    int32 mConsoleWidth;
    int32 mConsoleHeight;

    // screen buffer
    bool bIsScreenUpdated;
    std::string mScreenBuffer;
    uint64 mScreenBufferIndex;
    bool bIsScreenBufferRemain;
    
    

};

}
