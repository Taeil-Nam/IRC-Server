#pragma once

#include "common.hpp"
#include <vector>
#include <sys/time.h>

#include "grc/animation/IAnimation.hpp"

namespace grc
{

class Earth : public IAnimation
{
public:
    Earth();
    Earth(const Earth& copy);
    Earth& operator=(const Earth& copy);
    virtual ~Earth();

    virtual void Print(gdf::DisplayConsole& monitor);
private:
    uint64 mFramesIndex;
    std::vector<std::string> mFrames;
    struct timeval mLastUpdateTime;
};

} // namespace grc