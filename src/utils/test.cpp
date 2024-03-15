

#include <cstring>
#include <iostream>
#include <sys/_types/_ssize_t.h>
#include <sys/fcntl.h>
#include <unistd.h>
int main()
{
    fcntl(1, F_SETFL, O_NONBLOCK);
    char* str = (char *)malloc(1000*1000*2);
    memset(str, 's', sizeof(char) * 1000*1000*2);
    const ssize_t len = strlen(str);
    std::cerr << write(1, str, len) << " " << len << std::endl;
}