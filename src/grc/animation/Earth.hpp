#pragma once

#include "common.hpp"
#include <vector>

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

    virtual void PrintNextFrame(gdf::DisplayConsole& monitor);
private:
    enum { FRAMES_SIZE = 30};
    uint64 mFramesIndex;
    std::vector<std::string> mFrames;

};

} // namespace grc