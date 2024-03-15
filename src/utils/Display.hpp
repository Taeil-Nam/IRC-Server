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
