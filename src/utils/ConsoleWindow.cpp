#include "ConsoleWindow.hpp"

namespace grc
{

ConsoleWindow::ConsoleWindow()
{
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(NULL);
    setTerminalMode(true);
    mANSIColors[Default] = "\033[0m";
    mANSIColors[Cyan] = "\033[36m";
    mANSIColors[Red] = "\033[31m";
    mANSIColors[Green] = "\033[32m";
    mANSIColors[BrightBlue] = "\033[34;1m";
    mANSIColors[WhiteCharBlueBG] = "\033[44;37m";
    mANSIColors[WhiteCharRedBG] = "\033[41;37m";
    mHeaderColor = mANSIColors[WhiteCharBlueBG];
    mHeader = "Untitled";
    mFooterColor = mANSIColors[WhiteCharBlueBG];
    mFooter = "";
    bIsTimestampEnabled = true;
    mOutputBufferCapacity = 100;
    updateConsoleSize();
}

ConsoleWindow::~ConsoleWindow()
{
    setTerminalMode(false);
    printClear();
}

ConsoleWindow::ConsoleWindow(const ConsoleWindow& copy)
{
    mHeaderColor = copy.mHeaderColor;
    mHeader = copy.mHeader;
    mFooterColor = copy.mFooterColor;
    mFooter = copy.mFooter;
    mANSIColors = copy.mANSIColors;
    mCommandLineBuffer = copy.mCommandLineBuffer;
    mInputBuffer = copy.mInputBuffer;
    mOutputBuffer = copy.mOutputBuffer;
    mConsoleWidth = copy.mConsoleWidth;
    mConsoleHeight = copy.mConsoleHeight;
    bIsTimestampEnabled = copy.bIsTimestampEnabled;
    mOutputBufferCapacity = copy.mOutputBufferCapacity;
}

ConsoleWindow& ConsoleWindow::operator=(const ConsoleWindow& copy)
{
    if (this != &copy)
    {
        mHeaderColor = copy.mHeaderColor;
        mHeader = copy.mHeader;
        mFooterColor = copy.mFooterColor;
        mFooter = copy.mFooter;
        mANSIColors = copy.mANSIColors;
        mCommandLineBuffer = copy.mCommandLineBuffer;
        mInputBuffer = copy.mInputBuffer;
        mOutputBuffer = copy.mOutputBuffer;
        mConsoleWidth = copy.mConsoleWidth;
        mConsoleHeight = copy.mConsoleHeight;
        bIsTimestampEnabled = copy.bIsTimestampEnabled;
        mOutputBufferCapacity = copy.mOutputBufferCapacity;
    }
    return *this;
}

void ConsoleWindow::PushBackCommandLine(const char c)
{
    if (c == '\n' || c == '\x04')
    {
        mInputBuffer.push_back(mCommandLineBuffer);
        mCommandLineBuffer.clear();
    }
    else if (c == 127)
    {
        if (mCommandLineBuffer.empty() == false)
            mCommandLineBuffer.pop_back();
    }
    else if (c == '\033')
    {
        getchar(); getchar();
    }
    else
    {
        mCommandLineBuffer.push_back(c);
    }
}

std::string ConsoleWindow::In()
{
    ASSERT(mInputBuffer.size());
    std::string res = mInputBuffer.front();
    mInputBuffer.pop_front();
    return res;
}

void ConsoleWindow::Out(const std::string& str, eANSIColor color)
{
    OutputFormat toPush;
    toPush.TimeStamp = getCurrentTimeString();
    toPush.String = str;
    toPush.Color = color;
    mOutputBuffer.push_back(toPush);
    if (mOutputBuffer.size() > mOutputBufferCapacity)
        mOutputBuffer.pop_front();
}

bool ConsoleWindow::IsEOF() const
{
    if (mInputBuffer.empty()) return true;
    else return false;
}

void ConsoleWindow::RefreshConsole()
{
    updateConsoleSize();
    printClear();
    if (mConsoleHeight < 8 || mConsoleWidth < 18)
    {
        printErrorScreenFull();
    }
    else
    {
        printHeader();
        printOutput();
        printFooter();
        printCommandLine();
    }
    printFlush();
}

void ConsoleWindow::SetHeaderColor(const eANSIColor color)
{
    mHeaderColor = mANSIColors[color];
}

void ConsoleWindow::SetFooterColor(const eANSIColor color)
{
    mFooterColor = mANSIColors[color];
}

void ConsoleWindow::SetHeader(const std::string& str)
{
    mHeader = str;
}

void ConsoleWindow::SetFooter(const std::string& str)
{
    mFooter = str;
}

void ConsoleWindow::SetTimestamp(const bool enable)
{
    bIsTimestampEnabled = enable;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ------------------------------- private ---------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void ConsoleWindow::setTerminalMode(bool enable)
{
    static struct termios oldt, newt;
    if (enable)
    {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON);
        newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

std::string ConsoleWindow::getCurrentTimeString() const
{
    time_t currentTime = time(0);
    struct tm *localTime = localtime(&currentTime);
    int32 hour = localTime->tm_hour;
    int32 minute = localTime->tm_min;
    std::ostringstream timeStream;
    timeStream << std::setfill('0') << std::setw(2) << hour << ":"
               << std::setfill('0') << std::setw(2) << minute;
    return timeStream.str();
}

void ConsoleWindow::updateConsoleSize()
{
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
    mConsoleWidth = window.ws_col;
    mConsoleHeight = window.ws_row;
}

void ConsoleWindow::locateCursorToLine(const int32 line) const
{
    std::cout << "\033[" << line << ";1H" << std::flush;
}

uint64 ConsoleWindow::strlenMultiByte(const std::string& str) const
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

void ConsoleWindow::printClear() const
{
    std::cout << "\033[H\033[J";
}

void ConsoleWindow::printErrorScreenFull() const
{
    std::cout << mANSIColors.at(WhiteCharBlueBG)
              << std::string(mConsoleHeight * mConsoleWidth, ' ')
              << mANSIColors.at(Default);
}

void ConsoleWindow::printHeader() const
{
    const uint64 headerWidth = mHeader.size();
    if (headerWidth + 1 <= mConsoleWidth)
    {
        uint64 emptyLength = mConsoleWidth - (headerWidth + 1);
        std::cout << mHeaderColor << ' ' << mHeader
                    << std::string(emptyLength, ' ')
                    << mANSIColors.at(Default) << '\n';
    }
    else
    {
        std::cout << mHeaderColor << ' '
                    << mHeader.substr(0, mConsoleWidth - 1)
                    << mANSIColors.at(Default) << '\n';
    }
}

void ConsoleWindow::printOutput() const
{
    const uint64 leftHeight = mConsoleHeight - 3;
    uint64 outBufferIndex = 0;
    if (mOutputBuffer.size() > leftHeight)
        outBufferIndex = mOutputBuffer.size() - leftHeight;
    for (uint64 i = outBufferIndex; i < mOutputBuffer.size(); ++i)
    {
        if (bIsTimestampEnabled)
        {
            std::cout << mOutputBuffer[i].TimeStamp << ' '
                      << mANSIColors.at(BrightBlue) << '-'
                      << mANSIColors.at(Default) << '!'
                      << mANSIColors.at(BrightBlue) << '-'
                      << mANSIColors.at(Default) << ' ';
        }
        if (strlenMultiByte(mOutputBuffer[i].String)
            + (bIsTimestampEnabled ? 10 : 0)
                > mConsoleWidth)
        {
            const uint64 leftWidth = mConsoleWidth
                                     - (bIsTimestampEnabled ? 10 : 0);
            std::cout << mANSIColors.at(mOutputBuffer[i].Color);
            const char* c_str = mOutputBuffer[i].String.c_str();
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
            std::cout << "...\n" << mANSIColors.at(Default);
        }
        else
        {
            std::cout << mANSIColors.at(mOutputBuffer[i].Color)
                      << mOutputBuffer[i].String << '\n'
                      << mANSIColors.at(Default); 
        }
    }
}

void ConsoleWindow::printFooter() const
{  
    locateCursorToLine(mConsoleHeight - 1);
    std::cout << mFooterColor
                << mANSIColors.at(Cyan) <<" ["
                << mFooterColor << getCurrentTimeString()
                << mANSIColors.at(Cyan) <<"] "
                << std::string(mConsoleWidth - 9,' ')
                << mANSIColors.at(Default);
}

void ConsoleWindow::printCommandLine() const
{
    locateCursorToLine(mConsoleHeight);
    if (mCommandLineBuffer.size() + 11 <= mConsoleWidth)
    {
        std::cout << "[(status)] " << mCommandLineBuffer;
    }
    else
    {
        uint64 leftWidth = mConsoleWidth - 11;
        std::cout << "[(status)] "
                  << mCommandLineBuffer.substr(mCommandLineBuffer.size()
                                               - leftWidth);
    }
}

void ConsoleWindow::printFlush() const
{
    std::cout << std::flush;
}

}