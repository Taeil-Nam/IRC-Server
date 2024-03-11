#pragma once

#include <deque>
#include <map>

#include "common.hpp"

#include "Display.hpp"

namespace grc
{

class DisplayConsole
{
public:
    DisplayConsole(const std::string& IN header, const std::string& IN footer,
                   Display::eColor IN headerColor, Display::eColor IN footerColor)
    : bIsTimestampEnabled(true)
    {
        mANSIColors[Display::Default] = "\033[0m";
        mANSIColors[Display::Cyan] = "\033[36m";
        mANSIColors[Display::Red] = "\033[31m";
        mANSIColors[Display::Green] = "\033[32m";
        mANSIColors[Display::BrightBlue] = "\033[34;1m";
        mANSIColors[Display::WhiteCharBlueBG] = "\033[44;37m";
        mANSIColors[Display::WhiteCharRedBG] = "\033[41;37m";
        mDisplay.SetHeader(header);
        mDisplay.SetFooter(footer);
        mDisplay.SetContentBufferCapacity(1024);
    }

    void PushCharPrompt(const char IN ch)
    {
        if (ch == '\n' || ch == '\x04')
        {
            mPromptQueue.push_back(mPromptBuffer);
            mPromptBuffer.clear();
        }
        else if (ch == 127)
        {
            if (mPromptBuffer.size() > 0)
                mPromptBuffer.pop_back();
        }
        else if (ch == '\033')
        {
            getchar(); getchar();
        }
        else
        {
            mPromptBuffer.push_back(ch);
        }
    }

    bool pollPromptQueue(std::string& OUT prompt)
    {
        if (mPromptQueue.empty())
            return false;
        prompt = mPromptQueue.front();
        mPromptQueue.pop_front();
        return true;
    }

    void PushContent(const std::string& IN content, Display::eColor IN color)
    {
        mDisplay.PushContent(content, color);
    }

    void ClearContent()
    {
        mDisplay.Clear();
    }

    void RenderConsole(std::string& OUT consoleFrameBuffer)
    {

    }

    void SetHeader(const std::string& IN str)
    {
        mDisplay.SetHeader(str);
    }

    void SetFooter(const std::string& IN str)
    {
        mDisplay.SetFooter(str);
    }

    void SetHeaderColor(const Display::eColor IN color)
    {
        mHeaderColor = color;
    }

    void SetFooterColor(const Display::eColor IN color)
    {
        mFooterColor = color;
    }

    void SetTimestamp(const bool IN enable)
    {
        bIsTimestampEnabled = enable;
    }

    
    
private:
    Display mDisplay;
    std::map<Display::eColor, std::string> mANSIColors;
    std::string mHeaderColor;
    std::string mFooterColor;
    bool bIsTimestampEnabled;

    std::string mPromptBuffer;
    std::deque<std::string> mPromptQueue;

};

}
