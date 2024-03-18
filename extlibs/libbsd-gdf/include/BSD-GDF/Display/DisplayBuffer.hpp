#pragma once

#include <ctime>
#include <iostream>
#include <deque>
#include <sstream>
#include <iomanip>

#include "../Config.hpp"

namespace gdf
{

class DisplayBuffer
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
    DisplayBuffer();
    virtual ~DisplayBuffer();
    DisplayBuffer(const DisplayBuffer& copy);
    DisplayBuffer& operator=(const DisplayBuffer& IN copy);
    void Clear();
    void PushContent(const std::string& IN str, DisplayBuffer::eColor IN color);
    const std::deque<DisplayBuffer::Content>& GetContentBuffer() const;
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
