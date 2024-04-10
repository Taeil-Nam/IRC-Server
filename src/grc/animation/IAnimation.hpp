#pragma once

#include "BSD-GDF/Display/DisplayConsole.hpp"
#include "common.hpp"

namespace grc
{

interface IAnimation
{
    virtual ~IAnimation() {}
    virtual void PrintNextFrame(gdf::DisplayConsole& monitor) = 0;
};

}
