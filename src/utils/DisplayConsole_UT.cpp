#include "DisplayConsole.hpp"
#include "Display.hpp"
#include <unistd.h>

using namespace grc;

int main()
{
    DisplayConsole monitor("monitor", "time", Display::WhiteCharBlueBG, Display::WhiteCharBlueBG);
    monitor.PushContent("hello", Display::Default);
    monitor.PushCharPrompt('c');
    while (1)
    {
        monitor.NonBlockWrite();
        
        monitor.PushCharPrompt('h');
        monitor.PushCharPrompt('h');
        usleep(1000 * 16);
    }
    return 0;
}