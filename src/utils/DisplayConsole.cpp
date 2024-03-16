#include "DisplayConsole.hpp"

namespace grc
{

uint64 DisplayConsole::sStaticInstanceCount = 0;
struct termios DisplayConsole::sStaticOldTerminal;
DisplayConsole::DisplayConsole(const std::string& IN header,
                               const std::string& IN footer,
                               const Display::eColor IN headerColor,
                               const Display::eColor IN footerColor)
: bIsTimestampEnabled(true)
, bIsScreenUpdated(true)
, bIsScreenBufferRemain(true)
, mScreenBufferIndex(0)
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
    mHeaderColor = mANSIColors[headerColor];
    mFooterColor = mANSIColors[footerColor];
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
    bIsScreenUpdated = true;
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
    bIsScreenUpdated = true;
    mDisplay.PushContent(content, color);
}

void DisplayConsole::ClearContent()
{
    bIsScreenUpdated = true;
    mDisplay.Clear();
}

void DisplayConsole::SetHeader(const std::string& IN str)
{
    bIsScreenUpdated = true;
    mDisplay.SetHeader(str);
}

void DisplayConsole::SetFooter(const std::string& IN str)
{
    bIsScreenUpdated = true;
    mDisplay.SetFooter(str);
}

void DisplayConsole::SetHeaderColor(const Display::eColor IN color)
{
    bIsScreenUpdated = true;
    mHeaderColor = mANSIColors[color];
}

void DisplayConsole::SetFooterColor(const Display::eColor IN color)
{
    bIsScreenUpdated = true;
    mFooterColor = mANSIColors[color];
}

void DisplayConsole::SetTimestamp(const bool IN enable)
{
    bIsScreenUpdated = true;
    bIsTimestampEnabled = enable;
}

void DisplayConsole::ScreenNonBlockWrite()
{
    if (bIsScreenUpdated || updateConsoleSize())
    {
        renderScreenString(mScreenBuffer);
        mScreenBufferIndex = 0;
        bIsScreenBufferRemain = true;
        bIsScreenUpdated = false;
    }
    if (bIsScreenBufferRemain)
    {
        const char *buf = mScreenBuffer.c_str();
        const uint64 len = std::strlen(&buf[mScreenBufferIndex]);
        const int64 wrote = write(1, &buf[mScreenBufferIndex], len);
        mScreenBufferIndex += wrote;
        if (wrote == len)
        {
            bIsScreenBufferRemain = false;
            mScreenBufferIndex = 0;
        }
    }
}

void DisplayConsole::SetIsScreenUpdated(bool status)
{
    bIsScreenUpdated = status;
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

bool DisplayConsole::updateConsoleSize()
{
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
    if (mConsoleWidth != window.ws_col || mConsoleHeight != window.ws_row)
    {
        mConsoleWidth = window.ws_col;
        mConsoleHeight = window.ws_row;
        return true;
    }
    else
    {
        return false;
    }
}

std::string DisplayConsole::cursorToLine(const int32 line) const
{
    std::stringstream ss; ss << line;
    return std::string("\033[" + ss.str() + ";1H");
}

uint64 DisplayConsole::strlenMultiByte(const std::string& str) const
{
    const char* c_str = str.c_str();
    uint64 res = 0, index = 0;
    while (c_str[index])
    {
        if (c_str[index] < 0)
        {
            index += 3; res += 1;
        }
        else
        {
            index += 1; res += 1;
        }
    }
    return res;
}

void DisplayConsole::renderScreenString(std::string& OUT screenBuffer)
{
    updateConsoleSize();
    screenBuffer.clear();
    screenBuffer = "\033[H\033[J"; // clear;
    if (mConsoleHeight < 8 || mConsoleWidth < 18)
    {
        return ;
    }
    else
    {
        appendHeader(screenBuffer);
        appendContent(screenBuffer);
        appendFooter(screenBuffer);
        appendPrompt(screenBuffer);
    }
}

void DisplayConsole::appendHeader(std::string& OUT screenBuffer)
{

    const uint64 headerWidth = mDisplay.GetHeader().size();
    if (headerWidth + 1 <= mConsoleWidth)
    {
        uint64 emptyLength = mConsoleWidth - (headerWidth + 1);
        screenBuffer += mHeaderColor + ' ' + mDisplay.GetHeader()
                           + std::string(emptyLength, ' ')
                           + mANSIColors.at(Display::Default) + '\n';
    }
    else
    {
        screenBuffer += mHeaderColor + ' '
                           + mDisplay.GetHeader().substr(0, mConsoleWidth - 1)
                           + mANSIColors.at(Display::Default) + '\n';
    }
}

void DisplayConsole::appendContent(std::string& OUT screenBuffer)
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
            screenBuffer += contentBuffer[i].TimeStamp + ' '
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
            screenBuffer += mANSIColors.at(contentBuffer[i].Color);
            const char* c_str = contentBuffer[i].String.c_str();
            for (uint64 j = 0, strIndex = 0; j < leftWidth - 3; ++j)
            {
                if (c_str[strIndex] < 0)
                {
                    screenBuffer += c_str[strIndex++];
                    screenBuffer += c_str[strIndex++];
                    screenBuffer += c_str[strIndex++];
                }
                else
                {
                    screenBuffer += c_str[strIndex++];
                }

            }
            screenBuffer += "...\n" + mANSIColors.at(Display::Default);
        }
        else
        {
            screenBuffer += mANSIColors.at(contentBuffer[i].Color)
                               + contentBuffer[i].String + '\n'
                               + mANSIColors.at(Display::Default); 
        }
    }    
}

void DisplayConsole::appendFooter(std::string& OUT screenBuffer)
{
    screenBuffer += cursorToLine(mConsoleHeight - 1);
    const uint64 Footer = mDisplay.GetFooter().size();
    if (Footer + 1 <= mConsoleWidth)
    {
        uint64 emptyLength = mConsoleWidth - (Footer + 1);
        screenBuffer += mFooterColor + ' ' + mDisplay.GetFooter()
                           + std::string(emptyLength, ' ')
                           + mANSIColors.at(Display::Default) + '\n';
    }
    else
    {
        screenBuffer += mFooterColor + ' '
                           + mDisplay.GetFooter().substr(0, mConsoleWidth - 1)
                           + mANSIColors.at(Display::Default) + '\n';
    }
}

void DisplayConsole::appendPrompt(std::string& OUT screenBuffer)
{
    screenBuffer += cursorToLine(mConsoleHeight);
    if (mPromptBuffer.size() + 11 <= mConsoleWidth)
    {
        screenBuffer += "[(status)] " + mPromptBuffer;
    }
    else
    {
        uint64 leftWidth = mConsoleWidth - 11;
        screenBuffer += "[(status)] "
                           + mPromptBuffer.substr(mPromptBuffer.size()
                                                  - leftWidth);
    }
}

}