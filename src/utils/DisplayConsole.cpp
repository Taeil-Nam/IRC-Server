#include "DisplayConsole.hpp"
#include "types.hpp"
#include "utils/Display.hpp"
#include <sstream>

namespace grc
{

int32 DisplayConsole::sStaticInstanceCount = 0;

DisplayConsole::DisplayConsole(const std::string& IN header,
                               const std::string& IN footer,
                               const Display::eColor IN headerColor,
                               const Display::eColor IN footerColor)
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
    if (sStaticInstanceCount == 0)
    {
        setTerminalMode(true);
    }
    ++sStaticInstanceCount;
}

DisplayConsole::~DisplayConsole()
{
    --sStaticInstanceCount;
    if (sStaticInstanceCount == 0)
    {
        setTerminalMode(false);
    }
}

void DisplayConsole::PushCharPrompt(const char IN ch)
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

bool DisplayConsole::pollPromptQueue(std::string& OUT prompt)
{
    if (mPromptQueue.empty())
        return false;
    prompt = mPromptQueue.front();
    mPromptQueue.pop_front();
    return true;
}

void DisplayConsole::PushContent(const std::string& IN content, Display::eColor IN color)
{
    mDisplay.PushContent(content, color);
}

void DisplayConsole::ClearContent()
{
    mDisplay.Clear();
}

void DisplayConsole::SetHeader(const std::string& IN str)
{
    mDisplay.SetHeader(str);
}

void DisplayConsole::SetFooter(const std::string& IN str)
{
    mDisplay.SetFooter(str);
}

void DisplayConsole::SetHeaderColor(const Display::eColor IN color)
{
    mHeaderColor = mANSIColors[color];
}

void DisplayConsole::SetFooterColor(const Display::eColor IN color)
{
    mFooterColor = mANSIColors[color];
}

void DisplayConsole::SetTimestamp(const bool IN enable)
{
    bIsTimestampEnabled = enable;
}

void DisplayConsole::RenderScreenString(std::string& OUT screenStringBuffer)
{
    screenStringBuffer.clear();
    screenStringBuffer = "\033[H\033[J"; // clear;
    if (mConsoleHeight < 8 || mConsoleWidth < 18)
    {
        return ;
    }
    else
    {

    }

}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ------------------------------- private ---------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void DisplayConsole::setTerminalMode(const bool IN enable)
{
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(NULL);
    struct termios newTerminal;
    if (enable)
    {
        ASSERT(sStaticInstanceCount == 0);
        tcgetattr(STDIN_FILENO, &sStaticOldTerminal);
        newTerminal = sStaticOldTerminal;
        newTerminal.c_lflag &= ~(ICANON);
        newTerminal.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newTerminal);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &sStaticOldTerminal);
    }
}

void DisplayConsole::updateConsoleSize()
{
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
    mConsoleWidth = window.ws_col;
    mConsoleHeight = window.ws_row;
}

void DisplayConsole::appendHeader(std::string& OUT consoleFrameBuffer)
{

    const uint64 headerWidth = mDisplay.GetHeader().size();
    if (headerWidth + 1 <= mConsoleWidth)
    {
        uint64 emptyLength = mConsoleWidth - (headerWidth + 1);
        consoleFrameBuffer += mHeaderColor + ' ' + mDisplay.GetHeader()
                           + std::string(emptyLength, ' ')
                           + mANSIColors.at(Display::Default) + '\n';
    }
    else
    {
        consoleFrameBuffer += mHeaderColor + ' '
                           + mDisplay.GetHeader().substr(0, mConsoleWidth - 1)
                           + mANSIColors.at(Display::Default) + '\n';
    }
}

void DisplayConsole::appendContent(std::string& OUT consoleFrameBuffer)
{
    const uint64 leftHeight = mConsoleHeight - 3;
    uint64 outBufferIndex = 0;
    const std::deque<Display::Content>& contentBuffer = mDisplay.GetContentBuffer();
    if (contentBuffer.size() > leftHeight)
        outBufferIndex = contentBuffer.size() - leftHeight;
    for (uint64 i = outBufferIndex; i < contentBuffer.size(); ++i)
    {
        if (bIsTimestampEnabled)
        {
            consoleFrameBuffer += contentBuffer[i].TimeStamp + ' '
                               + mANSIColors.at(Display::BrightBlue) + '-'
                               + mANSIColors.at(Display::Default) + '!'
                               + mANSIColors.at(Display::BrightBlue) + '-'
                               + mANSIColors.at(Display::Default) + ' ';
        }
        if (strlenMultiByte(contentBuffer[i].String)
            + (bIsTimestampEnabled ? 10 : 0)
                > mConsoleWidth)
        {
            const uint64 leftWidth = mConsoleWidth
                                     - (bIsTimestampEnabled ? 10 : 0);
            std::cout << mANSIColors.at(contentBuffer[i].Color);
            const char* c_str = contentBuffer[i].String.c_str();
            for (uint64 j = 0, strIndex = 0; j < leftWidth - 3; ++j)
            {
                if (c_str[strIndex] < 0)
                {
                    std::cout << c_str[strIndex++];
                    std::cout << c_str[strIndex++];
                    std::cout << c_str[strIndex++];
                }
                else
                {
                    std::cout << c_str[strIndex++];
                }

            }
            std::cout << "...\n" << mANSIColors.at(Display::Default);
        }
        else
        {
            std::cout << mANSIColors.at(contentBuffer[i].Color)
                      << contentBuffer[i].String << '\n'
                      << mANSIColors.at(Display::Default); 
        }
    }    
}

}