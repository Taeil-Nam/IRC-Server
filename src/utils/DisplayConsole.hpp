#pragma once


#include <sys/termios.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <deque>
#include <map>

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

    void RenderScreenString(std::string& OUT consoleFrameBuffer);

private:
    void setTerminalMode(const bool IN enable);
    void updateConsoleSize();

    void appendHeader(std::string& OUT consoleFrameBuffer);
    void appendContent(std::string& OUT consoleFrameBuffer);
    void appendFooter(std::string& OUT consoleFrameBuffer);
    void appendPrompt(std::string& OUT consoleFrameBuffer);
private:
    static uint64 sStaticInstanceCount;
    static struct termios sStaticOldTerminal;
    

    Display mDisplay;
    std::map<Display::eColor, std::string> mANSIColors;
    std::string mHeaderColor;
    std::string mFooterColor;
    bool bIsTimestampEnabled;

    std::string mPromptBuffer;
    std::deque<std::string> mPromptQueue;

    int32 mConsoleWidth;
    int32 mConsoleHeight;

};

}
