#include "../../include/BSD-GDF/Display/DisplayBuffer.hpp"
namespace gdf
{

DisplayBuffer::DisplayBuffer()
: mHeader("HEADER")
, mFooter("FOOTER")
, mContentBufferCapacity(1024)
, mContentBuffer()
{}

DisplayBuffer::~DisplayBuffer()
{}

DisplayBuffer::DisplayBuffer(const DisplayBuffer& IN copy)
: mHeader(copy.mHeader)
, mFooter(copy.mFooter)
, mContentBufferCapacity(copy.mContentBufferCapacity)
, mContentBuffer(copy.mContentBuffer)
{}

DisplayBuffer& DisplayBuffer::operator=(const DisplayBuffer& IN copy)
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

void DisplayBuffer::Clear()
{
    mContentBuffer.clear();
}

void DisplayBuffer::PushContent(const std::string& IN str, DisplayBuffer::eColor IN color)
{
    Content toPush;
    mContentBuffer.push_back(toPush);
    mContentBuffer.back().String = str;
    getCurrentTimeString(mContentBuffer.back().TimeStamp);
    mContentBuffer.back().Color = color;
    if (mContentBuffer.size() > mContentBufferCapacity)
        mContentBuffer.pop_front();
}

const std::deque<DisplayBuffer::Content>& DisplayBuffer::GetContentBuffer() const
{
    return mContentBuffer;
}

const std::string& DisplayBuffer::GetHeader() const
{
    return mHeader;
}

const std::string& DisplayBuffer::GetFooter() const
{
    return mFooter;
}

void DisplayBuffer::SetHeader(const std::string& IN str)
{
    mHeader = str;
}

void DisplayBuffer::SetFooter(const std::string& IN str)
{
    mFooter = str;
}

void DisplayBuffer::SetContentBufferCapacity(const uint64 IN capacity)
{
    mContentBufferCapacity = capacity;
}

void DisplayBuffer::getCurrentTimeString(std::string& OUT str)
{
    std::time_t currentTime = std::time(0);
    struct std::tm* localTime = std::localtime(&currentTime);
    std::ostringstream timeStream;
    timeStream << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
               << std::setfill('0') << std::setw(2) << localTime->tm_min;
    str = timeStream.str();
}

}
