#include "DisplayConsole.hpp"
#include "Display.hpp"
#include <unistd.h>

using namespace grc;

int main()
{
    DisplayConsole monitor("monitor", "time", Display::WhiteCharBlueBG, Display::WhiteCharBlueBG);
    std::string buf;

    while (1)
    {
        monitor.RenderScreenString(buf);
        
        std::cout << buf <<  std::endl;
        usleep(1000 * 16);
    }
    return 0;
}