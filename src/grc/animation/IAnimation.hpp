#pragma once

#include "BSD-GDF/Display/DisplayConsole.hpp"
#include "common.hpp"

namespace grc
{

interface IAnimation
{
    virtual ~IAnimation() {}
    virtual void Print(gdf::DisplayConsole& monitor) = 0;
};

}
