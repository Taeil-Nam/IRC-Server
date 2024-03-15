#include "Display.hpp"

int main()
{
    grc::Display a;

    a.SetHeader("test_header");
    a.SetFooter("test_footer");
    a.PushContent("hello1");
    a.PushContent("hello2");
    a.PushContent("hello3");
    a.PushContent("hello4");
    a.PushContent("hello5");
    std::cout << a.GetHeader() << std::endl;
    const std::deque<grc::Display::Content>& buf = a.GetContentBuffer();
    for (grc::Display::Content c : buf)
    {
        std::cout << c.TimeStamp << ", " << c.String << std::endl;
    }
    std::cout << a.GetFooter() << std::endl;

    std::cout << "\n Clear \n" << std::endl;
    a.Clear();

    std::cout << a.GetHeader() << std::endl;
    for (grc::Display::Content c : buf)
    {
        std::cout << c.TimeStamp << ", " << c.String << std::endl;
    }
    std::cout << a.GetFooter() << std::endl;
    
    return 0;
}