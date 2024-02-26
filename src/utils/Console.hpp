#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <map>
#include <deque>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "AssertManager.hpp"

namespace grc
{

class Console
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
    Console();
    ~Console();
    void PutCharToCursor(const char c);
    void Out(const std::string& str, const eANSIColor color = Default);
    bool InEOF() const;
    std::string In();
    void RefreshScreen() const;
    void SetHeaderColor(const eANSIColor color);
    void SetFooterColor(const eANSIColor color);
    void SetHeader(const std::string& str);
    void SetFooter(const std::string& str);
    void SetTimeStamp(const bool b);
private:
    struct OutputFormat
    {
        std::string TimeStamp;
        std::string String;
        eANSIColor Color;
    };
    void setTerminalMode(const bool enable);
    std::string currentTimeString() const;
    void updateConsoleSize() const;
    void goToLine(const int32_t line) const;
    uint64_t strlenMultiByte(const std::string& str) const;
private:
    std::string mHeaderColor;
    std::string mHeader;
    std::string mFooterColor;
    std::string mFooter;
    std::map<int32_t, std::string> mANSIColors;
    std::string mInputLine;
    std::deque<std::string> mInputBuffer;
    std::deque<OutputFormat> mOutputBuffer;
    int16_t mConsoleWidth;
    int16_t mConsoleHeight;
    bool bTimeStamp;
};

}



/* namespace grc
{

class Console
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

    Console()
    {
        std::ios_base::sync_with_stdio(false);
        std::cout.tie(nullptr);
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
        mFooter = currentTimeString();
        bTimeStamp = true;
        updateConsoleSize();
    }

    ~Console()
    {
        setTerminalMode(false);
        system("clear");
    }

    void PushInputLine(char c)
    {
        if (c == '\n' || c == '\x04')
        {
            mInputBuffer.push_back(mInputLine);
            mInputLine.clear();
        }
        else if (c == 127)
        {
            if (mInputLine.empty() == false)
                mInputLine.pop_back();
        }
        else if (c == '\033')
        {
            getchar();
            getchar();
        }
        else
        {
            mInputLine.push_back(c);
        }
        
        return ;
    }

    void Out(const std::string& str, eANSIColor color = Default)
    {
        
        OutputFormat toPush;
        toPush.TimeStamp = currentTimeString();
        toPush.String = str;
        toPush.Color = color; 
        mOutputBuffer.push_back(toPush);
        if (mOutputBuffer.size() > 50)
            mOutputBuffer.pop_front();
    }
    
    std::string In()
    {
        ASSERT(mInputBuffer.empty() == false);
        std::string res = mInputBuffer.front();
        mInputBuffer.pop_front();
        return res;
    }

    bool InEOF()
    {
        if (mInputBuffer.empty()) return true;
        else return false;
    }

    void RefreshScreen()
    {
        updateConsoleSize();
        std::cout << "\033[H\033[J";
        if (mConsoleHeight < 7 || mConsoleWidth < 17)
        {
            std::cout << mANSIColors[WhiteCharBlueBG] 
                      << std::string(mConsoleHeight * mConsoleWidth, ' ')
                      << mANSIColors[Default]
                      <<std::flush;
            return;
        }

        // Header
        uint64_t headerWidth = mHeader.length();
        if (headerWidth + 1 <= mConsoleWidth)
        {
            uint64_t emptyLength = mConsoleWidth - (headerWidth + 1);
            std::cout << mHeaderColor << ' ' << mHeader
                      << std::string(emptyLength, ' ')
                      << mANSIColors[Default] << '\n';
        }
        else
        {
            std::cout << mHeaderColor << ' '
                      << mHeader.substr(0, mConsoleWidth - 1)
                      << mANSIColors[Default] << '\n';
        }

        // Message
        uint64_t leftHeight = mConsoleHeight - 3;
        uint64_t outBufferIndex = 0;
        if (mOutputBuffer.size() > leftHeight)
            outBufferIndex = mOutputBuffer.size() - leftHeight;
        for (uint64_t i = outBufferIndex; i < mOutputBuffer.size(); ++i)
        {
            // Time stamp
            if (bTimeStamp)
            {
                std::cout << mOutputBuffer[i].TimeStamp << ' '
                        << mANSIColors[BrightBlue] << '-'
                        << mANSIColors[Default] << '!'
                        << mANSIColors[BrightBlue] << '-'
                        << mANSIColors[Default] << ' ';
            }
            // String
            if (strlenMultiByte(mOutputBuffer[i].String) + (bTimeStamp ? 10 : 0) > mConsoleWidth)
            {
                uint64_t leftWidth = mConsoleWidth - (bTimeStamp ? 10 : 0);
                std::cout << mANSIColors[mOutputBuffer[i].Color];
                const char* c_str = mOutputBuffer[i].String.c_str();
                for (uint64_t j = 0, strIndex = 0; j < leftWidth - 3; ++j)
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
                std::cout << "...\n" << mANSIColors[Default];
            }
            else
            {
                std::cout << mANSIColors[mOutputBuffer[i].Color]
                          << mOutputBuffer[i].String << '\n'
                          << mANSIColors[Default]; 
            }
        }
        
        // Footer
        goToLine(mConsoleHeight - 1);
        std::cout << mFooterColor
                  << mANSIColors[Cyan]<<" ["
                  << mFooterColor << currentTimeString()
                  << mANSIColors[Cyan] <<"] "
                  << std::string(mConsoleWidth - 9,' ')
                  << mANSIColors[Default];
        
        // Input line
        goToLine(mConsoleHeight);
        if (mInputLine.length() + 11 <= mConsoleWidth)
        {
            std::cout << "[(status)] " << mInputLine;
        }
        else
        {
            uint64_t leftWidth = mConsoleWidth - 11;
            std::cout << "[(status)] " << mInputLine.substr(mInputLine.length() - leftWidth);
        }
        std::cout << std::flush;
    }
    
    void SetHeaderColor(eANSIColor color)
    {
        mHeaderColor = mANSIColors[color];
    }

    void SetHeader(const std::string& str)
    {
        mHeader = str;
    }

    void SetFooterColor(eANSIColor color)
    {
        mFooterColor = mANSIColors[color];
    }

    void SetFooter(const std::string& str)
    {
        mFooter = str;
    }

    void SetTimeStamp(bool b)
    {
        bTimeStamp = b;
    }
    
private:
    struct OutputFormat
    {
        std::string TimeStamp;
        std::string String;
        eANSIColor Color;
    };

    void setTerminalMode(bool enable)
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

    std::string currentTimeString()
    {
        time_t currentTime = time(0);
        struct tm *localTime = localtime(&currentTime);
        int32_t hour = localTime->tm_hour;
        int32_t minute = localTime->tm_min;
        std::ostringstream timeStream;
        timeStream << std::setfill('0') << std::setw(2) << hour << ":"
                   << std::setfill('0') << std::setw(2) << minute;
        return timeStream.str();
    }

    void updateConsoleSize()
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        mConsoleWidth = w.ws_col;
        mConsoleHeight = w.ws_row;
    }

    void goToLine(int32_t line)
    {
        std::cout << "\033[" << line << ";1H" << std::flush;
    }

    uint64_t strlenMultiByte(const std::string& str)
    {
        const char* c_str = str.c_str();
        uint64_t res = 0;
        uint64_t index = 0;
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

private:
    std::string mHeaderColor;
    std::string mHeader;
    std::string mFooterColor;
    std::string mFooter;
    std::map<int32_t, std::string> mANSIColors;

    std::string mInputLine;
    std::deque<std::string> mInputBuffer;
    std::deque<OutputFormat> mOutputBuffer;

    int16_t mConsoleWidth;
    int16_t mConsoleHeight;

    bool bTimeStamp;
};

} */