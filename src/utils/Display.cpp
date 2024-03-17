#include "Display.hpp"

namespace grc
{

Display::Display()
: mHeader("HEADER")
, mFooter("FOOTER")
, mContentBufferCapacity(1024)
, mContentBuffer()
{}

Display::~Display()
{}

Display::Display(const Display& IN copy)
: mHeader(copy.mHeader)
, mFooter(copy.mFooter)
, mContentBufferCapacity(copy.mContentBufferCapacity)
, mContentBuffer(copy.mContentBuffer)
{}

Display& Display::operator=(const Display& IN copy)
{
    if (this != &copy)
    {
        mHeader = copy.mHeader;
        mFooter = copy.mFooter;
        mContentBufferCapacity = copy.mContentBufferCapacity;
        mContentBuffer = copy.mContentBuffer;
    }
    return *this;
}

void Display::Clear()
{
    mContentBuffer.clear();
}

void Display::PushContent(const std::string& IN str, Display::eColor IN color)
{
    Content toPush;
    mContentBuffer.push_back(toPush);
    mContentBuffer.back().String = str;
    getCurrentTimeString(mContentBuffer.back().TimeStamp);
    mContentBuffer.back().Color = color;
    if (mContentBuffer.size() > mContentBufferCapacity)
        mContentBuffer.pop_front();
}

const std::deque<Display::Content>& Display::GetContentBuffer() const
{
    return mContentBuffer;
}

const std::string& Display::GetHeader() const
{
    return mHeader;
}

const std::string& Display::GetFooter() const
{
    return mFooter;
}

void Display::SetHeader(const std::string& IN str)
{
    mHeader = str;
}

void Display::SetFooter(const std::string& IN str)
{
    mFooter = str;
}

void Display::SetContentBufferCapacity(const uint64 IN capacity)
{
    mContentBufferCapacity = capacity;
}

void Display::getCurrentTimeString(std::string& OUT str)
{
    time_t currentTime = time(0);
    struct tm* localTime = localtime(&currentTime);
    std::ostringstream timeStream;
    timeStream << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
               << std::setfill('0') << std::setw(2) << localTime->tm_min;
    str = timeStream.str();
}

}
